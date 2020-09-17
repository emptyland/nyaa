#include "ui/label-check-box.h"
#include "resource/font-library.h"
#include "game/game.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

namespace ui {

LabelCheckBox::LabelCheckBox(Id id, std::string_view name, Component *parent /* = nullptr*/) : Component(id, parent) {
    set_name(std::string(name.data(), name.size()));
}

LabelCheckBox::LabelCheckBox(Id id, res::TextID name, Component *parent /* = nullptr*/) : Component(id, parent) {
    SetName(name);
}

LabelCheckBox::~LabelCheckBox() {}

void LabelCheckBox::HandleMouseButtonInput(int button, int action, int mods, bool *should_break) {
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            if (action == GLFW_PRESS) {
                for (auto [deg, _] : *mutable_delegates()) {
                    down_cast<Delegate>(deg)->OnCommand(this, id());
                    down_cast<Delegate>(deg)->OnChecked(this, &checked_);
                }
                if (delegates().empty()) {
                    checked_ = !checked_;
                }
                *should_break = true;
            }
            break;

        default: break;
    }
}

void LabelCheckBox::OnMouseMove(double x, double y) {
    // TODO:
}

void LabelCheckBox::OnPaint(double delta) {
    glBegin(GL_LINE_LOOP);
    glColor3f(1, 1, 1);

    int box_h = bound().h / 4 * 3;

    glVertex2f(bound().x, bound().y);
    glVertex2f(bound().x, bound().y + box_h);
    glVertex2f(bound().x + box_h, bound().y + box_h);
    glVertex2f(bound().x + box_h, bound().y);

    glVertex2f(bound().x + 2, bound().y + 2);
    glVertex2f(bound().x + 2, bound().y + box_h - 2);
    glVertex2f(bound().x + box_h - 2, bound().y + box_h - 2);
    glVertex2f(bound().x + box_h - 2, bound().y + 2);

    glEnd();

    if (checked()) {
        glBegin(GL_QUADS);
        glColor3f(1, 1, 1);

        int core_h = bound().h / 2;
        int x      = bound().x + (box_h - core_h) / 2;
        int y      = bound().y + (box_h - core_h) / 2;

        glVertex2f(x, y);
        glVertex2f(x, y + core_h);
        glVertex2f(x + core_h, y + core_h);
        glVertex2f(x + core_h, y);

        glEnd();
    }

    Vector2f size = font()->ApproximateSize(name()) * 0.8f;

    std::vector<float> vertices;

    font()->Render(Vec3(bound().x + bound().h, bound().y + (bound().h - size.y) / 2, 0), 0.8, name(), &vertices);

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
}

}  // namespace ui

}  // namespace nyaa