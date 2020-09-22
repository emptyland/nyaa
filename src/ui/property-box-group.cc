#include "ui/property-box-group.h"
#include "ui/label-component.h"
#include "resource/font-library.h"
#include "game/game.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

namespace ui {
PropertyBoxGroup::PropertyBoxGroup(Id id, std::string_view name, Component *parent) : Component(id, name, parent) {}
PropertyBoxGroup::PropertyBoxGroup(Id id, res::TextID name, Component *parent) : Component(id, name, parent) {}
PropertyBoxGroup::~PropertyBoxGroup() {}

LabelPropertyBox *PropertyBoxGroup::AddPropertyBox(Id id, std::string_view name) {
    LabelPropertyBox *child = new LabelPropertyBox(id, name, this);
    child->SetEnable(true);
    child->SetVisible(true);
    return child;
}

LabelPropertyBox *PropertyBoxGroup::AddPropertyBox(Id id, res::TextID name) {
    LabelPropertyBox *child = new LabelPropertyBox(id, name, this);
    child->SetEnable(true);
    child->SetVisible(true);
    return child;
}

void PropertyBoxGroup::OnPaint(double delta) {
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

    glBegin(GL_LINES);
    glColor4f(kBorderColor.x, kBorderColor.y, kBorderColor.z, kBorderColor.w);

    glVertex2f(bound().x, y);
    glVertex2f(bound().x + bound().w, y);

    glVertex2f(bound().x, y - 4);
    glVertex2f(bound().x + bound().w, y - 4);
    glEnd();

    y -= 8;

    int item_h = (bound().h - (font_size.y * font_scale() + 16 + ((children().size() - 1) * padding_size()))) /
                 children().size();
    y -= item_h;
    for (Component *child : *mutable_children()) {
        LabelPropertyBox *cb = down_cast<LabelPropertyBox>(child);

        int value = cb->Child()->value();
        if (data_) {
            data_->OnPropertyBoxProduce(this, cb->id(), &value);
            cb->Child()->set_value(value);
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