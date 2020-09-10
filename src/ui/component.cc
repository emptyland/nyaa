#include "ui/component.h"
#include "game/game.h"
#include "glog/logging.h"
#include <GL/glew.h>
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

void Component::DrawBorder(double delta) {
    glBegin(GL_QUADS);
    glColor4f(bg_color().x, bg_color().y, bg_color().z, bg_color().w);
    glVertex2f(bound().x, bound().y);
    glVertex2f(bound().x + bound().w, bound().y);
    glVertex2f(bound().x + bound().w, bound().y + bound().h);
    glVertex2f(bound().x, bound().y + bound().h);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(1.0, 1.0, 1.0);
    glVertex2f(bound().x, bound().y);
    glVertex2f(bound().x + bound().w, bound().y);
    glVertex2f(bound().x + bound().w, bound().y + bound().h);
    glVertex2f(bound().x, bound().y + bound().h);

    glVertex2f(bound().x + 2, bound().y + 2);
    glVertex2f(bound().x + bound().w - 2, bound().y + 2);
    glVertex2f(bound().x + bound().w - 2, bound().y + bound().h - 2);
    glVertex2f(bound().x + 2, bound().y + bound().h - 2);
    glEnd();
}

}  // namespace ui

}  // namespace nyaa
