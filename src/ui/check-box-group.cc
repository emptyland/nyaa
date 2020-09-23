#include "ui/check-box-group.h"
#include "resource/font-library.h"
#include "game/game.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

namespace ui {

CheckBoxGroup::CheckBoxGroup(Id id, std::string_view name, Component *parent) : Component(id, name, parent) {}
CheckBoxGroup::CheckBoxGroup(Id id, res::TextID name, Component *parent) : Component(id, name, parent) {}
CheckBoxGroup::~CheckBoxGroup() {}

LabelCheckBox *CheckBoxGroup::AddCheckBox(Id id, std::string_view name) {
    LabelCheckBox *cb = (new LabelCheckBox(id, name, this));
    cb->set_font_scale(font_scale());
    cb->SetEnable(true);
    cb->SetVisible(true);
    return cb;
}

LabelCheckBox *CheckBoxGroup::AddCheckBox(Id id, res::TextID name) {
    LabelCheckBox *cb = (new LabelCheckBox(id, name, this));
    cb->set_font_scale(font_scale());
    cb->SetEnable(true);
    cb->SetVisible(true);
    return cb;
}

void CheckBoxGroup::OnPaint(double delta) {
    Vector2f font_size = font()->ApproximateSize(name());

    float y = bound().y + bound().h - font_size.y * font_scale();

    std::vector<float> vertices;
    font()->Render(Vec3(bound().x + 4, y - 4, 0), font_scale(), name(), &vertices);

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

    y -= 4;

    glLineWidth(2.0);
    glBegin(GL_LINES);
    glColor4f(kBorderColor.x, kBorderColor.y, kBorderColor.z, kBorderColor.w);

    glVertex2f(bound().x, y);
    glVertex2f(bound().x + bound().w, y);

    glVertex2f(bound().x, y - 4);
    glVertex2f(bound().x + bound().w, y - 4);
    glEnd();
    glLineWidth(1.0);

    y -= 8;

    int item_h =
        (bound().h - (font_size.y * font_scale() + 16 + ((children().size() - 1) * padding_size()))) / children().size();
    y -= item_h;
    for (Component *child : *mutable_children()) {
        LabelCheckBox *cb = down_cast<LabelCheckBox>(child);

        bool value = cb->checked();
        if (data_) {
            data_->OnCheckBoxProduce(this, cb->id(), &value);
            cb->set_checked(value);
        }

        cb->set_bound({
            bound().x,
            static_cast<int>(y),
            bound().w,
            item_h,
        });
        y -= (padding_size() + item_h);
    }
}

}  // namespace ui

}  // namespace nyaa