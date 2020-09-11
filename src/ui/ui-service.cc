#include "ui/ui-service.h"
#include "ui/button-group.h"
#include "ui/input-box.h"
#include "ui/list-box.h"
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

ButtonGroup *UIService::NewButtonGroup(int column, int row, Component *parent) {
    Component::Id id   = Component::Id::Of(next_id_++);
    ButtonGroup * ctrl = new ButtonGroup(id, column, row, parent);
    PutController(ctrl);
    return ctrl;
}

InputBox *UIService::NewInputBox(std::string_view text, Component *parent) {
    Component::Id id   = Component::Id::Of(next_id_++);
    InputBox *    ctrl = new InputBox(id, parent);
    PutController(ctrl);
    return ctrl;
}

ListBox *UIService::NewListBox(int limit_rows, Component *parent) {
    Component::Id id   = Component::Id::Of(next_id_++);
    ListBox *     ctrl = new ListBox(id, parent);
    ctrl->set_limit_rows(limit_rows);
    PutController(ctrl);
    return ctrl;
}

void UIService::Destroy(Component *ctrl) {
    id_to_ctrl_.erase(ctrl->id());
    if (!ctrl->parent()) {
        roots_.erase(std::find(roots_.begin(), roots_.end(), ctrl));
    } else {
        ctrl->parent()->mutable_children()->Remove(ctrl);
    }
    delete ctrl;
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

void UIService::HandleCharInput(unsigned int codepoint, bool *did) {
    if (focus_) {
        double time = Game::This()->ts();

        if (last_codepoint_ == codepoint && time - last_time_ < 0.07) { return; }
        last_time_      = time;
        last_codepoint_ = codepoint;
        focus_->HandleCharInput(codepoint, did);
    }
}

void UIService::HandleKeyEvent(bool *did) {
    if (focus_) { focus_->HandleKeyEvent(did); }
}

void UIService::HandleMouseEvent(bool *did) {
    auto win = Game::This()->window();

    double x = 0, y = 0;
    glfwGetCursorPos(win, &x, &y);

    y = Game::This()->fb_h() - y * dpi_factor_;
    x *= dpi_factor_;

    if (last_mouse_pos_.x != x || last_mouse_pos_.y != y) {
        for (Component *ctrl : roots_) {
            if (!ctrl->IsVisible()) { continue; }
            for (Component *node : *ctrl->mutable_children()) {
                if (node->IsEnable() && node->IsVisible()) { node->OnMouseMove(x, y); }
            }
            ctrl->OnMouseMove(x, y);
        }
        last_mouse_pos_.x = x;
        last_mouse_pos_.y = y;
    }

    if (focus_ && InBound<int>(focus_->bound(), x, y)) {
        focus_->HandleMouseEvent(x, y, did);
        if (*did) { return; }
    }

    bool pressed = glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ||
                   glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
    if (!pressed) { return; }

    if (focus_) {
        if (!InBound<int>(focus_->bound(), x, y)) {
            focus_->SetFocus(false);
            focus_->DidFocus(false);
            focus_ = nullptr;
        }
    }

    for (Component *ctrl : roots_) {
        if (!ctrl->IsEnable() || !ctrl->IsVisible()) { continue; }
        if (ctrl != focus_ && InBound<int>(ctrl->bound(), x, y)) {
            if (ctrl->children().empty()) {
                focus_ = ctrl;
                focus_->SetFocus(true);
                focus_->DidFocus(true);
                break;
            }
            for (Component *node : *ctrl->mutable_children()) {
                if (!node->IsEnable() || !node->IsVisible()) { continue; }
                if (node != focus_ && InBound<int>(node->bound(), x, y)) {
                    focus_ = node;
                    focus_->SetFocus(true);
                    focus_->DidFocus(true);
                    return;
                }
            }
        }
    }

    if (focus_) { focus_->HandleMouseEvent(x, y, did); }
}

void UIService::Render(double delta) {
    Game::This()->transform()->Enter2DProjection();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (Component *ctrl : roots_) {
        if (ctrl->IsVisible()) {
            ctrl->OnPaint(delta);
            for (Component *node : *ctrl->mutable_children()) {
                if (node->IsVisible()) { node->OnPaint(delta); }
            }
        }
    }

    glDisable(GL_BLEND);
    Game::This()->transform()->Exit2DProjection();
}

void UIService::PutController(Component *ctrl) {
    if (!ctrl->parent()) { roots_.push_back(ctrl); }
    id_to_ctrl_[ctrl->id()] = ctrl;
    ctrl->SetEnable(true);
    ctrl->SetVisible(true);
}

}  // namespace ui

}  // namespace nyaa
