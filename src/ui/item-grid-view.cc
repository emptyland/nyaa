#include "ui/item-grid-view.h"
#include "resource/font-library.h"
#include "resource/texture-library.h"
#include "game/game.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

namespace ui {
// 82.0	53.0	29.0
ItemGridView::ItemGridView(Id id, int column_count, int row_count, Component *parent)
    : Component(id, parent)
    , column_count_(column_count)
    , row_count_(row_count)
    , cursor_({column_count_, row_count_})
    , drag_({{0, 0}, 0, nullptr})
    , items_(new Item[column_count * row_count]) {
    DCHECK_GT(column_count_ * row_count_, 0);
}

ItemGridView::~ItemGridView() {}

void ItemGridView::Drop(ItemGridView *sender, Vector2i src, int stack, bool *dropped) {
    Vector2i pos = GetCursorPosition();
    if (!InBound(bound(), pos.x, pos.y)) { return; }

    const int dy = (bound().h - 4) / row_count_;
    const int dx = (bound().w - 4) / column_count_;

    cursor_.x = std::min(static_cast<int>((pos.x - bound().x) / dx), column_count_ - 1);
    cursor_.y = std::min(static_cast<int>((pos.y - bound().y) / dy), row_count_ - 1);

    if (!data_) { return; }

    DCHECK(!is_drag());
    Drag(ToUserGrid(src), stack, sender);
    *dropped = true;
}

void ItemGridView::HandleMouseButtonInput(int key, int action, int mods, bool *should_break) {
    Vector2i  pos = GetCursorPosition();
    const int dy  = (bound().h - 4) / row_count_;
    const int dx  = (bound().w - 4) / column_count_;

    cursor_.x = std::min(static_cast<int>((pos.x - bound().x) / dx), column_count_ - 1);
    cursor_.y = std::min(static_cast<int>((pos.y - bound().y) / dy), row_count_ - 1);

    if (!data_ || cursor_.x < 0 || cursor_.y < 0) { return; }

    Item *grid = item(cursor_.x, cursor_.y);
    data_->OnItemProduce(this, ToUserGrid(cursor_), grid);
    switch (key) {
        case GLFW_MOUSE_BUTTON_RIGHT:
            if (action != GLFW_PRESS) { return; }
            if (is_drag()) { return; }
            if (grid->icon()) {
                for (auto [deg, _] : *mutable_delegates()) {
                    bool dropped = true;
                    down_cast<Delegate>(deg)->DidItemDispose(this, ToUserGrid(cursor_), &dropped);
                }
                *should_break = true;
            }
            break;

        case GLFW_MOUSE_BUTTON_LEFT:
            if (action != GLFW_PRESS) { return; }
            if (!is_drag()) {
                if (grid->icon()) { Drag(cursor_, TestKeyPress(GLFW_KEY_LEFT_SHIFT) ? 1 : grid->stack(), this); }
                *should_break = true;
                return;
            }

            if (TestKeyPress(GLFW_KEY_LEFT_SHIFT)) {
                DCHECK(is_drag());
                if (drag_.grid == cursor_ && drag_.stack < grid->stack()) {
                    drag_.stack++;
                    *should_break = true;
                    return;
                }
            }

            for (auto [deg, _] : *mutable_delegates()) {
                bool dropped = true;
                down_cast<Delegate>(deg)->OnItemDrop(drag_.sender, ToUserGrid(drag_.grid), this, ToUserGrid(cursor_),
                                                     drag_.stack, &dropped);
                if (dropped) { Drop(); }
                break;
            }
            *should_break = true;
            break;

        default: break;
    }
}

void ItemGridView::DidFocus(bool focus) {
    if (!focus && is_drag()) {
        // Item *stub = item(drag_.x, drag_.y);
        for (auto [deg, _] : *mutable_delegates()) {
            bool dropped = false;
            down_cast<Delegate>(deg)->OnItemDrop(this, ToUserGrid(drag_.grid), drag_.stack, &dropped);
            if (dropped) {
                Drop();
                break;
            }
        }
    }
}

void ItemGridView::OnPaint(double delta) {
    DrawBorder(border_color_, delta);

    if (!font()) { set_font(Game::This()->font_lib()->default_face()); }

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

            Item *grid = item(i, j);
            if (!data_) { continue; }
            data_->OnItemProduce(this, {i, row_count_ - 1 - j}, grid);
            if (!grid->icon()) { continue; }

            if (drag_.grid.x == i && drag_.grid.y == j) {
                glColor4f(0.7, 0.7, 0.7, 0.5);
                DrawIcon(Rect<float>(x, y, dx, dy), grid->icon(), grid->stack() - drag_.stack);
            } else {
                glColor3f(1.0, 1.0, 1.0);
                DrawIcon(Rect<float>(x, y, dx, dy), grid->icon(), grid->stack());
            }
        }
    }

    if (is_drag()) {
        Vector2i pos  = GetCursorPosition();
        Item *   grid = drag_.sender->item(drag_.grid.x, drag_.grid.y);

        pos.x -= dx / 2;
        pos.y -= dy / 2;

        glColor3f(1.0, 1.0, 1.0);
        DrawIcon(Rect<float>(pos.x, pos.y, dx, dy), grid->icon(), drag_.stack);
    }
}

void ItemGridView::DrawIcon(const Boundf &bound, res::Texture *tex, int stack) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex->tex_id());

    glBegin(GL_QUADS);
    glTexCoord2fv(&tex->coord(0).x);
    glVertex2f(bound.x, bound.y);

    glTexCoord2fv(&tex->coord(1).x);
    glVertex2f(bound.x, bound.y + bound.h);

    glTexCoord2fv(&tex->coord(2).x);
    glVertex2f(bound.x + bound.w, bound.y + bound.h);

    glTexCoord2fv(&tex->coord(3).x);
    glVertex2f(bound.x + bound.w, bound.y);
    glEnd();

    if (stack > 1) {
        char buf[64];
        ::snprintf(buf, arraysize(buf), "%d", stack);
        Vector2f font_size  = font()->ApproximateSize(buf);
        float    font_scale = bound.h / 3 / font_size.y;

        glBindTexture(GL_TEXTURE_2D, 0);
        std::vector<float> vertices;
        Vector3f           pos;
        pos.x = bound.x + 4;
        pos.y = bound.y + 4;
        pos.z = 0;
        font()->Render(pos, font_scale, buf, &vertices);
        glBindTexture(GL_TEXTURE_2D, font()->buffered_tex());

        glBegin(GL_QUADS);
        glColor4fv(&font_color_.x);
        for (int i = 0; i < vertices.size(); i += 5) {
            glTexCoord2f(vertices[i + 3], vertices[i + 4]);
            glVertex3f(vertices[i + 0], vertices[i + 1], vertices[i + 2]);
        }
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}

}  // namespace ui

}  // namespace nyaa
