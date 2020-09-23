#include "ui/property-box.h"
#include "resource/font-library.h"
#include "game/game.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

namespace ui {

PropertyBox::PropertyBox(Id id, Component *parent) : Component(id, parent) {}
PropertyBox::~PropertyBox() {}

void PropertyBox::HandleMouseButtonInput(int button, int action, int mods, bool *should_break) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        switch (cursor_) {
            case kIncr:
                for (auto [deg, _] : *mutable_delegates()) { down_cast<Delegate>(deg)->OnIncreaseProperty(this); }
                break;
            case kDecr:
                for (auto [deg, _] : *mutable_delegates()) { down_cast<Delegate>(deg)->OnDecreaseProperty(this); }
                break;
            default: break;
        }
    }
}

void PropertyBox::OnMouseMove(double x, double y) {
    cursor_ = kNone;
    if (!InBound<int>(bound(), x, y)) { return; }

    if (x >= bound().x + bound().w - bound().h) {
        if (y >= bound().y + bound().h / 2) {
            cursor_ = kIncr;
        } else {
            cursor_ = kDecr;
        }
    }
}

void PropertyBox::OnPaint(double delta) {
    if (data_) { data_->OnPropertyProduce(this, &value_); }

    glBegin(GL_LINES);
    glVertex2i(bound().x, bound().y);
    glVertex2i(bound().x + bound().w - bound().h, bound().y);
    glEnd();

    char buf[64];
    ::snprintf(buf, arraysize(buf), "%d", value());
    Boundf rect = DrawLabel(Vec3(bound().x + 2, bound().y + 2 + font_bearing(), 0), buf);

    time_ += delta;
    uint64_t mills = time_ * 1000;
    if (IsFocus() && mills % 1000 > 500) {
        rect.x += rect.w;
        rect.y = bound().y + 4;
        rect.h = bound().h - 8;
        glBegin(GL_QUADS);
        glColor3f(1, 1, 1);
        glVertex2f(rect.x, rect.y);
        glVertex2f(rect.x + 4, rect.y);
        glVertex2f(rect.x + 4, rect.y + rect.h);
        glVertex2f(rect.x, rect.y + rect.h);
        glEnd();
    }

    int x = bound().x + bound().w - bound().h;
    if (cursor_ == kIncr) {
        glBegin(GL_TRIANGLES);
    } else {
        glBegin(GL_LINE_LOOP);  // incr
    }
    glColor3f(1.0, 1.0, 1.0);
    glVertex2i(x, bound().y + bound().h / 2 + 4);
    glVertex2i(x + bound().h / 2, bound().y + bound().h / 2 + 4);
    glVertex2i(x + bound().h / 4, bound().y + bound().h - 8);
    glEnd();

    if (cursor_ == kDecr) {
        glBegin(GL_TRIANGLES);
    } else {
        glBegin(GL_LINE_LOOP);  // decr
    }
    glColor3f(1.0, 1.0, 1.0);
    glVertex2i(x, bound().y + bound().h / 2 - 4);
    glVertex2i(x + bound().h / 2, bound().y + bound().h / 2 - 4);
    glVertex2i(x + bound().h / 4, bound().y + 8);
    glEnd();
}

}  // namespace ui

}  // namespace nyaa