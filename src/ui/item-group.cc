#include "ui/item-group.h"
#include "resource/font-library.h"
#include "resource/texture-library.h"
#include "game/game.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

namespace ui {
// 82.0	53.0	29.0
ItemGroup::ItemGroup(Id id, int column_count, int row_count, Component *parent)
    : Component(id, parent)
    , column_count_(column_count)
    , row_count_(row_count)
    , cursor_({column_count_, row_count_})
    , drag_({column_count_, row_count_})
    , items_(new Item[column_count * row_count]) {
    DCHECK_GT(column_count_ * row_count_, 0);
}

ItemGroup::~ItemGroup() {}

void ItemGroup::HandleMouseButtonInput(int key, int action, int mods, bool *should_break) {
    Vector2i  pos = GetCursorPosition();
    const int dy  = (bound().h - 4) / row_count_;
    const int dx  = (bound().w - 4) / column_count_;

    cursor_.x = std::min(static_cast<int>((pos.x - bound().x) / dx), column_count_ - 1);
    cursor_.y = std::min(static_cast<int>((pos.y - bound().y) / dy), row_count_ - 1);

    if (!data_) { return; }

    Item *stub = item(cursor_.x, cursor_.y);
    data_->OnItemProduce(cursor_.x, row_count_ - 1 - cursor_.y, stub);
    switch (key) {
        case GLFW_MOUSE_BUTTON_RIGHT:
            if (action != GLFW_PRESS) { return; }
            if (is_drag()) { return; }
            if (stub->icon()) {
                for (auto [deg, _] : *mutable_delegates()) {
                    bool dropped = true;
                    down_cast<Delegate>(deg)->DidItemDispose(ToUserGrid(cursor_), stub->id(), &dropped);
                }
                *should_break = true;
            }
            break;

        case GLFW_MOUSE_BUTTON_LEFT:
            if (action != GLFW_PRESS) { return; }
            if (!is_drag()) {
                if (stub->icon()) { drag_ = cursor_; }
                *should_break = true;
                return;
            }

            for (auto [deg, _] : *mutable_delegates()) {
                bool dropped = true;
                down_cast<Delegate>(deg)->OnItemDrop(ToUserGrid(drag_), ToUserGrid(cursor_), stub->id(), &dropped);
                if (dropped) { Drop(); }
                break;
            }
            *should_break = true;
            break;

        default: break;
    }
}

void ItemGroup::DidFocus(bool focus) {
    if (!focus && is_drag()) {
        Item *stub = item(drag_.x, drag_.y);
        for (auto [deg, _] : *mutable_delegates()) {
            bool dropped = true;
            down_cast<Delegate>(deg)->DidItemDispose(ToUserGrid(drag_), stub->id(), &dropped);
            if (dropped) {
                Drop();
                break;
            }
        }
    }
}

void ItemGroup::OnPaint(double delta) {
    DrawBorder(border_color_, delta);

    const int dx = (bound().w - 4) / column_count_;
    const int dy = (bound().h - 4) / row_count_;

    glBegin(GL_LINES);
    glColor4fv(&border_color_.x);
    for (int i = 1; i < row_count_; i++) {
        int y = bound().y + 2 + i * dy;
        glVertex2f(bound().x + 2, y);
        glVertex2f(bound().x + bound().w - 4, y);
    }
    for (int i = 1; i < column_count_; i++) {
        int x = bound().x + 2 + i * dx;
        glVertex2f(x, bound().y + 2);
        glVertex2f(x, bound().y + bound().h - 4);
    }
    glEnd();

    glColor3f(1.0, 1.0, 1.0);
    for (int j = 0; j < row_count_; j++) {
        for (int i = 0; i < column_count_; i++) {
            int x = bound().x + 2 + i * dx;
            int y = bound().y + 2 + j * dy;

            Item *stub = item(i, j);

            if (!data_) { continue; }

            data_->OnItemProduce(i, row_count_ - 1 - j, stub);

            if (!stub->icon()) { continue; }

            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, stub->icon()->tex_id());

            glBegin(GL_QUADS);
            glTexCoord2fv(&stub->icon()->coord(0).x);
            glVertex2f(x, y);

            glTexCoord2fv(&stub->icon()->coord(1).x);
            glVertex2f(x, y + dy);

            glTexCoord2fv(&stub->icon()->coord(2).x);
            glVertex2f(x + dx, y + dy);

            glTexCoord2fv(&stub->icon()->coord(3).x);
            glVertex2f(x + dx, y);

            glEnd();
            glDisable(GL_TEXTURE_2D);
        }
    }

    if (is_drag()) {
        Vector2i pos  = GetCursorPosition();
        Item *   stub = item(drag_.x, drag_.y);

        pos.x -= dx / 2;
        pos.y -= dy / 2;

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, stub->icon()->tex_id());

        glBegin(GL_QUADS);
        glTexCoord2fv(&stub->icon()->coord(0).x);
        glVertex2f(pos.x, pos.y);

        glTexCoord2fv(&stub->icon()->coord(1).x);
        glVertex2f(pos.x, pos.y + dy);

        glTexCoord2fv(&stub->icon()->coord(2).x);
        glVertex2f(pos.x + dx, pos.y + dy);

        glTexCoord2fv(&stub->icon()->coord(3).x);
        glVertex2f(pos.x + dx, pos.y);

        glEnd();
        glDisable(GL_TEXTURE_2D);
    }
}

}  // namespace ui

}  // namespace nyaa
