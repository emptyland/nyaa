#include "ui/flat-menu.h"
#include "resource/font-library.h"
#include "game/game.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

namespace ui {

FlatMenu::FlatMenu(Id id, Component *parent) : Component(id, parent), font_(Game::This()->font_lib()->default_face()) {
}

FlatMenu::~FlatMenu() {}

void FlatMenu::AddItem(std::string_view text, Id id) {
    DCHECK(FindItem(id) == items_.end());
    Item item;
    item.id            = id;
    item.text          = text;
    item.original_size = font_->ApproximateSize(text);
    item.bound         = {0, 0, 0, 0};
    items_.push_back(item);
}

void FlatMenu::OnMouseMove(double x, double y) {
    // TODO:
}

void FlatMenu::OnPaint(double delta) {
    Vector2f scale2 = ApproximateScale();
    float    scale  = std::min(scale2.x, scale2.y);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, font()->buffered_tex());

    int   index = 0;
    float y     = bound().y + bound().h;
    while (y >= bound().y && index < items_.size()) {
        Item *      item = &items_[index++];
        const float w    = item->original_size.x * scale;
        const float x    = bound().x + (bound().w - w) / 2;

        DrawText(x, y, scale, item);

        y -= (item->original_size.y + padding_size()) * scale;
    }

    glDisable(GL_TEXTURE_2D);
}

void FlatMenu::DrawText(float x, float y, float scale, Item *item) const {
    std::vector<float> vertices;

    item->bound = font()->Render({x, y, 0}, scale, item->text, &vertices);
    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 1.0);
    for (int i = 0; i < vertices.size(); i += 5) {
        glTexCoord2f(vertices[i + 3], vertices[i + 4]);
        glVertex3f(vertices[i + 0], vertices[i + 1], vertices[i + 2]);
    }
    glEnd();

}

Vector2f FlatMenu::ApproximateScale() const {
    Vector2f scale{0, 0};
    for (const auto &item : items_) {
        scale.y += item.original_size.y;
        scale.x = std::max(scale.x, item.original_size.x);
    }
    scale.y += padding_size() * (items_.size() - 1);
    scale.x = bound().w / scale.x;
    scale.y = bound().h / scale.y;
    return scale;
}

}  // namespace ui

}  // namespace nyaa