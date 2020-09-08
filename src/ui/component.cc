#include "ui/component.h"
#include "game/game.h"
#include <GLFW/glfw3.h>

namespace nyaa {

namespace ui {

Component::Component(Id id, Component *parent)
    : next_(this), prev_(this), id_(id), parent_(parent), last_time_(0) {
    if (parent_) { parent_->mutable_children()->Append(this); }
}

/*virtual*/ ComponentDelegate::~ComponentDelegate() {}

/*virtual*/ void ComponentDelegate::DidForce(Component *sender) {
    // Ignore
}

Component::~Component() {
    for (auto [value, ownership] : delegates()) {
        if (ownership) { delete value; }
    }
}

bool Component::DeltaTest(double delta) {
    if (Game::This()->ts() - last_time() > delta) {
        set_last_time(Game::This()->ts());
        return true;
    }
    return false;
}

/*virtual*/ void Component::HandleMouseEvent(double x, double y, bool *did) {}

/*virtual*/ void Component::HandleKeyEvent(bool *did) {}

/*virtual*/ void Component::HandleCharInput(char32_t code, bool *did) {}

/*virtual*/ void Component::DidFocus(bool focus) {}

/*virtual*/ void Component::OnMouseMove(double x, double y) {}

bool Component::TestKeyPress(int key) { return glfwGetKey(Game::This()->window(), key) == GLFW_PRESS; }

}  // namespace ui

}  // namespace nyaa
