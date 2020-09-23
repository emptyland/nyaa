#include "ui/ui-service.h"
#include "ui/component.h"
#include "game/game.h"
#include "game/vector.h"
#include "system/geometry-transform-system.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>

namespace nyaa {

namespace ui {

UIService::UIService(float dpi_factor) : last_time_(glfwGetTime()), dpi_factor_(dpi_factor) {}

UIService::~UIService() {
    for (auto [id, ctrl] : id_to_ctrl_) { delete ctrl; }
}

void UIService::Destroy(Component *ctrl) {
    id_to_ctrl_.erase(ctrl->id());
    if (!ctrl->parent()) {
        roots_.erase(std::find(roots_.begin(), roots_.end(), ctrl));
    } else {
        ctrl->parent()->mutable_children()->Remove(ctrl);
    }
    delete ctrl;
    if (focus_ == ctrl) { focus_ = nullptr; }
    if (modal_ == ctrl) { modal_ = nullptr; }
}

void UIService::SetFocus(Component *ctrl) {
    if (ctrl == focus_) { return; }

    if (focus_) {
        focus_->SetFocus(false);
        focus_->DidFocus(false);
    }

    focus_ = ctrl;
    if (focus_) {
        focus_->SetFocus(true);
        focus_->DidFocus(true);
    }
}

void UIService::HandleMouseMove() {
    double x = 0, y = 0;
    glfwGetCursorPos(Game::This()->window(), &x, &y);

    y = Game::This()->fb_h() - y * dpi_factor_;
    x *= dpi_factor_;

    if (x == last_mouse_pos_.x && y == last_mouse_pos_.y) { return; }
    last_mouse_pos_.x = x;
    last_mouse_pos_.y = y;

    if (modal()) {
        DispatchMouseMove(modal(), x, y);
    } else {
        for (Component *ctrl : roots_) { DispatchMouseMove(ctrl, x, y); }
    }
}

void UIService::DispatchMouseMove(Component *ctrl, double x, double y) {
    if (!ctrl->IsVisible()) { return; }
    for (Component *node : *ctrl->mutable_children()) { DispatchMouseMove(node, x, y); }
    ctrl->OnMouseMove(x, y);
}

void UIService::HandleMouseButtonInput(int button, int action, int mods, bool *should_break) {
    double x = 0, y = 0;
    glfwGetCursorPos(Game::This()->window(), &x, &y);

    y = Game::This()->fb_h() - y * dpi_factor_;
    x *= dpi_factor_;

    if (focus_ && InBound<int>(focus_->bound(), x, y)) {
        focus_->HandleMouseButtonInput(button, action, mods, should_break);
        if (*should_break) { return; }
    }

    if (focus_) {
        if (!InBound<int>(focus_->bound(), x, y)) {
            focus_->SetFocus(false);
            focus_->DidFocus(false);
            focus_ = nullptr;
        }
    }

    if (modal()) {
        if (!DoFocus(modal(), x, y)) {
            focus_ = nullptr;
        }
    } else {
        for (Component *ctrl : roots_) {
            if (DoFocus(ctrl, x, y)) { break; }
        }
    }
    if (focus_) { focus_->HandleMouseButtonInput(button, action, mods, should_break); }
}

bool UIService::DoFocus(Component *ctrl, double x, double y) {
    if (!ctrl->IsEnable() || !ctrl->IsVisible()) { return false; }

    if (ctrl != focus_ && InBound<int>(ctrl->bound(), x, y)) {
        if (ctrl->children().empty()) {
            focus_ = ctrl;
            focus_->SetFocus(true);
            focus_->DidFocus(true);
            return true;
        }
        for (Component *node : *ctrl->mutable_children()) {
            // if (!node->IsEnable() || !node->IsVisible()) { continue; }
            if (DoFocus(node, x, y)) { return true; }
        }
    }
    return false;
}

void UIService::HandleKeyInput(int key, int code, int action, int mods, bool *should_break) {
    if (focus_) {
        double time = Game::This()->ts();

        last_time_ = time;

        focus_->HandleKeyInput(key, code, action, mods, should_break);
        for (Component *node : *focus_->mutable_children()) {
            node->HandleKeyInput(key, code, action, mods, should_break);
        }
    }
}

void UIService::HandleCharInput(char32_t codepoint, bool *should_break) {
    if (focus_) {
        double time = Game::This()->ts();

        last_time_      = time;
        last_codepoint_ = codepoint;

        focus_->HandleCharInput(codepoint, should_break);
        for (Component *node : *focus_->mutable_children()) { node->HandleCharInput(codepoint, should_break); }
    }
}

void UIService::Render(double delta) {
    Game::This()->transform()->Enter2DProjection();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (Component *ctrl : roots_) { DoRender(ctrl, delta); }

    glDisable(GL_BLEND);
    Game::This()->transform()->Exit2DProjection();
}

void UIService::DoRender(Component *ctrl, double delta) {
    if (ctrl->IsVisible()) {
        ctrl->OnPaint(delta);
        for (Component *node : *ctrl->mutable_children()) {
            if (node->IsVisible()) { DoRender(node, delta); }
        }
    }
}

void UIService::PutController(Component *ctrl) {
    if (!ctrl->parent()) { roots_.push_back(ctrl); }
    id_to_ctrl_[ctrl->id()] = ctrl;
    ctrl->SetEnable(true);
    ctrl->SetVisible(true);
}

}  // namespace ui

}  // namespace nyaa
