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

/*virtual*/ void Controller::Render(double delta) {
    if (!IsEnable() || !IsVisible()) {
        return; // Ignore;
    }
    // TODO:
}

}  // namespace ui

}  // namespace nyaa
