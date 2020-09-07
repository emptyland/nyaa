#include "ui/ui-service.h"
#include "ui/input-box.h"
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

InputBox *UIService::NewInputBox(std::string_view text, Controller *parent) {
    Controller::Id id   = Controller::Id::Of(next_id_++);
    InputBox *     ctrl = new InputBox(id, parent);
    PutController(ctrl);
    return ctrl;
}

void UIService::Destroy(Controller *ctrl) {
    id_to_ctrl_.erase(ctrl->id());
    if (!ctrl->parent()) {
        roots_.erase(std::find(roots_.begin(), roots_.end(), ctrl));
    } else {
        ctrl->parent()->mutable_children()->Remove(ctrl);
    }
    delete ctrl;
}

void UIService::HandleCharInput(unsigned int codepoint, bool *did) {
    if (focus_) {
        if (last_codepoint_ == codepoint && glfwGetTime() - last_time_ < 0.1) { return; }
        last_time_      = glfwGetTime();
        last_codepoint_ = codepoint;
        focus_->HandleCharInput(codepoint, did);
    }
}

void UIService::HandleKeyEvent(bool *did) {
    if (focus_) {
        focus_->HandleKeyEvent(did);
    }
}

void UIService::HandleMouseEvent(bool *did) {
    auto win = Game::This()->window();

    double x = 0, y = 0;
    glfwGetCursorPos(win, &x, &y);

    y = Game::This()->fb_h() - y * dpi_factor_;
    x *= dpi_factor_;
    for (Controller *ctrl : roots_) {
        for (Controller *node : *ctrl->mutable_children()) {
            if (node->IsEnable() && node->IsVisible()) { node->OnMouseMove(x, y); }
        }
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

    for (Controller *ctrl : roots_) {
        if (ctrl != focus_ && InBound<int>(ctrl->bound(), x, y)) {
            if (ctrl->children().empty()) {
                focus_ = ctrl;
                focus_->SetFocus(true);
                focus_->DidFocus(true);
                break;
            }
            for (Controller *node : *ctrl->mutable_children()) {
                if (node != focus_ && InBound<int>(node->bound(), x, y)) {
                    focus_ = node;
                    focus_->SetFocus(true);
                    focus_->DidFocus(true);
                    return;
                }
            }
        }
    }
}

void UIService::Render(double delta) {
    Game::This()->transform()->Enter2DProjection();

    for (Controller *ctrl : roots_) {
        if (ctrl->IsVisible()) {
            ctrl->OnPaint(delta);
            for (Controller *node : *ctrl->mutable_children()) {
                if (node->IsVisible()) { node->OnPaint(delta); }
            }
        }
    }

    Game::This()->transform()->Exit2DProjection();
}

void UIService::PutController(Controller *ctrl) {
    if (!ctrl->parent()) { roots_.push_back(ctrl); }
    id_to_ctrl_[ctrl->id()] = ctrl;
    ctrl->SetEnable(true);
    ctrl->SetVisible(true);
}

}  // namespace ui

}  // namespace nyaa