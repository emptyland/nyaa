#include "ui/flat-menu.h"
#include "resource/font-library.h"
#include "game/game.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

namespace ui {

FlatMenu::FlatMenu(Id id, Component *parent) : Component(id, parent), font_(Game::This()->font_lib()->default_face()) {}

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
    float scale = std::min(scale2.x, scale2.y);

    int i = 0;
    float y = bound().y + bound().h;
    while (y >= bound().y) {
        Item *item = &items_[i++];
        const float w = item->original_size.x * scale;
        const float x = bound().x + (bound().w - w) / 2;

        // TODO:
        
        y -= item->original_size.y * scale;
    }
}

Vector2f FlatMenu::ApproximateScale() const {
    Vector2f scale{0, 0};
    for (const auto &item : items_) {
        scale.y += item.original_size.y;
        scale.x = std::max(scale.x, item.original_size.x);
    }
    scale.y += 48 * (items_.size() - 1);
    scale.x = bound().w / scale.x;
    scale.y = bound().h / scale.y;
    return scale;
}

}  // namespace ui

}  // namespace nyaa