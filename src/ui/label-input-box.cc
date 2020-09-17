#include "ui/label-input-box.h"
#include "ui/input-box.h"
#include "resource/font-library.h"
#include "game/game.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

namespace ui {

LabelInputBox::LabelInputBox(Id id, Component *parent) : Component(id, parent), child_(new InputBox(id, this)) {
    child_->SetVisible(true);
    child_->SetEnable(true);
    //AddChild(child_);
}

LabelInputBox::~LabelInputBox() {}

void LabelInputBox::OnPaint(double delta) {
    Vector2f size = font()->ApproximateSize(name()) * child_->font_scale();

    child_->set_bound({
        bound().x + 16 + static_cast<int>(size.x),
        bound().y,
        bound().w - 16 - static_cast<int>(size.x),
        bound().h,
    });

    std::vector<float> vertices;
    font()->Render(Vec3(bound().x, bound().y + child_->font_bearing(), 0), child_->font_scale(), name(),
                   &vertices);

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
