#include "ui/input-box.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

namespace ui {

InputBox::InputBox(Id id, Controller *parent /* = nullptr*/) : Controller(id, parent) {}

InputBox::~InputBox() {}

void InputBox::HandleKeyEvent(bool *did) {
    DLOG(INFO) << "key event!";
    *did = true;
}

void InputBox::DidFocus(bool focus) { DLOG(INFO) << "focus: " << focus; }

void InputBox::OnPaint(double /*delta*/) {
    glColor3f(1.0, 1.0, 0);
    glBegin(GL_LINE);
    glVertex2f(bound().x, bound().y);
    glVertex2f(bound().x + bound().w, bound().y);
    glVertex2f(bound().x + bound().w, bound().y + bound().h);
    glVertex2f(bound().x, bound().y + bound().h);
    glEnd();
}

}  // namespace ui

}  // namespace nyaa
