#include "ui/controller.h"

namespace nyaa {

namespace ui {

/*virtual*/ ControllerDelegate::~ControllerDelegate() {}

/*virtual*/ void ControllerDelegate::DidForce(Controller *sender) {
    // Ignore
}

Controller::~Controller() {
    for (auto [value, ownership] : delegates()) {
        if (ownership) { delete value; }
    }
}

/*virtual*/ void Controller::HandleMouseEvent(double x, double y, bool *did) {}

/*virtual*/ void Controller::HandleKeyEvent(bool *did) {}

/*virtual*/ void Controller::HandleCharInput(uint32_t code, bool *did) {}

/*virtual*/ void Controller::DidFocus(bool focus) {}

/*virtual*/ void Controller::OnMouseMove(double x, double y) {}


}  // namespace ui

}  // namespace nyaa
