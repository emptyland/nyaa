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
    Vector2i size      = font()->MakeOutlineTexture(text, Vec3(kFontColor), 3, Vec3(kFontOutlineColor), &item.tex);
    item.original_size = Vec2(size.x, size.y);
    item.bound         = {0, 0, 0, 0};
    items_.push_back(item);
}

void FlatMenu::HandleMouseButtonInput(int button, int action, int mods, bool *should_break) {
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            if (cursor_ < 0 || cursor_ >= items_.size()) { return; }
            if (action == GLFW_PRESS) {
                for (auto [deg, _] : *mutable_delegates()) { deg->OnCommand(this, items_[cursor_].id); }
            }
            break;

        default: break;
    }
}

void FlatMenu::OnMouseMove(double mx, double my) {
    cursor_ = -1;

    if (!InBound<int>(bound(), mx, my)) { return; }

    int   index = 0;
    float y     = bound().y + bound().h - items_.back().original_size.y * scale_ - padding_size();
    while (y >= bound().y && index < items_.size()) {
        Item *item = &items_[index++];

        if (my >= y && my <= y + item->original_size.y * scale_) {
            cursor_ = index - 1;
            break;
        }

        y -= (item->original_size.y + padding_size()) * scale_;
    }
}

void FlatMenu::OnPaint(double delta) {
    Vector2f scale = ApproximateScale();

    scale_ = std::min(scale.x, scale.y);

    glEnable(GL_TEXTURE_2D);

    int   index = 0;
    float y     = bound().y + bound().h - items_.back().original_size.y * scale_ - padding_size();
    while (y >= bound().y && index < items_.size()) {
        Item *      item = &items_[index++];
        const float w    = item->original_size.x * scale_;
        const float x    = bound().x + (bound().w - w) / 2;

        if (index - 1 == cursor_) {
            DrawText(x - item->original_size.x * 0.15, y - item->original_size.y * 0.15, scale_ + 0.3, item);
        } else {
            DrawText(x, y, scale_, item);
        }

        y -= (item->original_size.y) * scale_ + padding_size();
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