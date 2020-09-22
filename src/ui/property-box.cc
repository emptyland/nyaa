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

void PropertyBox::HandleMouseButtonInput(int button, int action, int mods, bool *should_break) {}
void PropertyBox::OnMouseMove(double x, double y) {}

void PropertyBox::OnPaint(double delta) {
    if (data_) { data_->OnPropertyProduce(this, &value_); }

    glBegin(GL_LINES);
    glVertex2i(bound().x, bound().y);
    glVertex2i(bound().x + bound().w, bound().y);
    glEnd();

    char buf[64];
    ::snprintf(buf, arraysize(buf), "%d", value());

    std::vector<float> vertices;

    Boundf rect = font()->Render(Vec3(bound().x + 2, bound().y + 2 + font_bearing(), 0), font_scale(), buf, &vertices);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, font()->buffered_tex());

    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 1.0);
    for (int i = 0; i < vertices.size(); i += 5) {
        glTexCoord2f(vertices[i + 3], vertices[i + 4]);
        glVertex3f(vertices[i + 0], vertices[i + 1], vertices[i + 2]);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);

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
}

}  // namespace ui

}  // namespace nyaa