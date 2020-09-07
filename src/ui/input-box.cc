#include "ui/input-box.h"
#include "game/game.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

namespace ui {

InputBox::InputBox(Id id, Controller *parent /* = nullptr*/) : Controller(id, parent) {}

InputBox::~InputBox() {}

void InputBox::HandleKeyEvent(bool *did) {
    if (glfwGetKey(Game::This()->window(), GLFW_KEY_ENTER) == GLFW_PRESS) {
        for (auto [deg, _] : *mutable_delegates()) { down_cast<Delegate>(deg)->DidForce(this); }
        *did = true;
        return;
    }

    if (glfwGetKey(Game::This()->window(), GLFW_KEY_BACKSPACE) == GLFW_PRESS) {

        *did = true;
        return;
    }
}

void InputBox::HandleCharInput(uint32_t code, bool *did) {
    DLOG(INFO) << "char input: " << code;
}

void InputBox::DidFocus(bool focus) {
    DLOG(INFO) << "focus: " << focus;
    for (auto [deg, _] : *mutable_delegates()) { deg->DidForce(this); }
}

void InputBox::OnPaint(double /*delta*/) {
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
    glVertex2f(bound().x, bound().y);
    glVertex2f(bound().x + bound().w, bound().y);
    glVertex2f(bound().x + bound().w, bound().y + bound().h);
    glVertex2f(bound().x, bound().y + bound().h);
    glEnd();
}

}  // namespace ui

}  // namespace nyaa
