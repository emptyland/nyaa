#include "ui/flat-menu.h"
#include "resource/font-library.h"
#include "game/game.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

namespace ui {

FlatMenu::FlatMenu(Id id, Component *parent) : Component(id, parent), font_(Game::This()->font_lib()->default_face()) {}

FlatMenu::~FlatMenu() {
    for (auto &item : items_) { glDeleteTextures(1, &item.tex); }
}

// 22.0	60.0	92.0	
void FlatMenu::AddItem(std::string_view text, Id id) {
    DCHECK(FindItem(id) == items_.end());
    Item item;
    item.id            = id;
    item.text          = text;
    Vector2i size      = font()->MakeOutlineTexture(text, Vec3(1, 1, 1), 3, Vec3(0.22, 0.6, 0.92), &item.tex);
    item.original_size = Vec2(size.x, size.y);
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
    item->bound.x = x;
    item->bound.y = y;
    item->bound.w = item->original_size.x * scale;
    item->bound.h = item->original_size.y * scale;

    glBindTexture(GL_TEXTURE_2D, item->tex);

    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 1.0);

    glTexCoord2f(0, 1);
    glVertex2f(x, y);

    glTexCoord2f(0, 0);
    glVertex2f(x, y + item->bound.h);

    glTexCoord2f(1, 0);
    glVertex2f(x + item->bound.w, y + item->bound.h);

    glTexCoord2f(1, 1);
    glVertex2f(x + item->bound.w, y);
    
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