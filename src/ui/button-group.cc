#include "ui/button-group.h"
#include "resource/font-library.h"
#include "game/game.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

namespace ui {

ButtonGroup::ButtonGroup(Id id, int column_count, int row_count, Component *parent)
    : Component(id, parent)
    , column_count_(column_count)
    , row_count_(row_count)
    , font_(Game::This()->font_lib()->default_face())
    , buttons_(new Button[column_count_ * row_count_]) {
    DCHECK_GT(column_count_ * row_count_, 0);
    cursor_.x = column_count_;
    cursor_.y = row_count_;
}

ButtonGroup::~ButtonGroup() {}

ButtonGroup::Button *ButtonGroup::AddButton(Id id, int column, int row) {
    DCHECK_GE(column, 0);
    DCHECK_LT(column, column_count_);
    DCHECK_GE(row, 0);
    DCHECK_LT(row, row_count_);

    Button *btn = &buttons_[row * column_count_ + column];
    if (btn->IsUsed()) { return nullptr; }
    btn->SetEnable(true);
    btn->SetUsed(true);
    btn->id_ = id;
    return btn;
}

void ButtonGroup::HandleKeyInput(int key, int code, int action, int mods, bool *should_break) {
    switch (key) {
        case GLFW_KEY_UP: {
            int y = cursor_.y - 1 < 0 ? row_count_ - 1 : (cursor_.y - 1) % row_count_;
            UpdateBtnFocus(cursor_.x == column_count_ ? 0 : cursor_.x, y);
        } break;

        case GLFW_KEY_DOWN:
            UpdateBtnFocus(cursor_.x == column_count_ ? 0 : cursor_.x, (cursor_.y + 1) % row_count_);
            break;

        case GLFW_KEY_LEFT:
            if (action == GLFW_PRESS || action == GLFW_REPEAT) {
                int x = cursor_.x - 1 < 0 ? column_count_ - 1 : (cursor_.x - 1) % column_count_;
                UpdateBtnFocus(x, cursor_.y == row_count_ ? 0 : cursor_.y);
            }
            break;

        case GLFW_KEY_RIGHT:
            if (action == GLFW_PRESS || action == GLFW_REPEAT) {
                UpdateBtnFocus((cursor_.x + 1) % column_count_, cursor_.y == row_count_ ? 0 : cursor_.y);
            }
            break;

        case GLFW_KEY_ENTER:
            if (action == GLFW_PRESS && cursor_.x != column_count_) {
                DCHECK_NE(cursor_.y, row_count_);

                Button *btn = button(cursor_.x, cursor_.y);
                for (auto [deg, _] : *mutable_delegates()) {
                    // down_cast<Delegate>(deg)->OnBtnPress(this, btn->id());
                    down_cast<Delegate>(deg)->OnCommand(this, btn->id());
                }
            }
            break;

        case GLFW_KEY_ESCAPE: UpdateBtnFocus(column_count_, row_count_); break;

        default: *should_break = false; return;
    }
    *should_break = true;
}

void ButtonGroup::HandleMouseButtonInput(int key, int action, int mods, bool *should_break) {
    if (cursor_.x == column_count_) {
        DCHECK_EQ(cursor_.y, row_count_);
        return;
    }

    if (key == GLFW_MOUSE_BUTTON_LEFT) {
        Button *btn = button(cursor_.x, cursor_.y);
        for (auto [deg, _] : *mutable_delegates()) { down_cast<Delegate>(deg)->OnCommand(this, btn->id()); }
        *should_break = true;
    }
}

void ButtonGroup::OnMouseMove(double x, double y) {
    if (cursor_.x != column_count_) {
        DCHECK_NE(cursor_.y, row_count_);
        button(cursor_.x, cursor_.y)->SetFocus(false);
    }

    if (!InBound<int>(bound(), x, y)) {
        cursor_.x = column_count_;
        cursor_.y = row_count_;
        return;
    }

    const int dy = (bound().h - 2) / row_count_;
    const int dx = (bound().w - 2) / column_count_;

    cursor_.x = std::min(static_cast<int>((x - bound().x) / dx), column_count_ - 1);
    cursor_.y = std::min(static_cast<int>((y - bound().y) / dy), row_count_ - 1);

    Button *btn = button(cursor_.x, cursor_.y);
    btn->SetFocus(true);
    for (auto [deg, _] : delegates()) { down_cast<Delegate>(deg)->DidBtnFocus(this, btn); }
}

void ButtonGroup::OnPaint(double delta) {
    if (!font()) { set_font(Game::This()->font_lib()->default_face()); }

    glBegin(GL_LINE_LOOP);
    glColor3f(0.7, 0.7, 0.7);
    glVertex2f(bound().x, bound().y);
    glVertex2f(bound().x + bound().w, bound().y);
    glVertex2f(bound().x + bound().w, bound().y + bound().h);
    glVertex2f(bound().x, bound().y + bound().h);
    glEnd();

    int i = 0;

    const int dy = (bound().h - 2) / row_count_;
    const int dx = (bound().w - 2) / column_count_;
    for (int y = bound().y + 1; y < bound().y + bound().h - 1; y += dy) {
        for (int x = bound().x + 1; x < bound().x + bound().w - 1; x += dx) {
            Button *btn = &buttons_[i++];
            if (!btn->IsUsed()) { continue; }

            glBegin(GL_QUADS);

            if (btn->IsFocus()) {
                glColor4fv(&btn->fg_color().x);
            } else {
                glColor4fv(&btn->bg_color().x);
            }
            glVertex2f(x, y);
            glVertex2f(x + dx, y);
            glVertex2f(x + dx, y + dy);
            glVertex2f(x, y + dy);
            glEnd();

            Vector2f font_size     = font()->ApproximateSize(btn->name());
            Vector3f font_position = Vec3(x + (dx - font_size.x) / 2, y + (dy - font_size.y) / 2, 0);

            std::vector<float> vertices;
            font()->Render(font_position, 0.8, btn->name(), &vertices);

            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, font()->buffered_tex());

            glBegin(GL_QUADS);
            glColor3fv(&btn->font_color().x);
            for (int i = 0; i < vertices.size(); i += 5) {
                glTexCoord2f(vertices[i + 3], vertices[i + 4]);
                glVertex3fv(&vertices[i + 0]);
            }
            glEnd();
            glDisable(GL_TEXTURE_2D);
        }
    }
}

void ButtonGroup::UpdateBtnFocus(int i, int j) {
    if (cursor_.x != column_count_) {
        DCHECK_NE(cursor_.y, row_count_);
        button(cursor_.x, cursor_.y)->SetFocus(false);
    }
    cursor_.x = i;
    cursor_.y = j;
    if (cursor_.x != column_count_) {
        DCHECK_NE(cursor_.y, row_count_);
        Button *btn = button(cursor_.x, cursor_.y);
        btn->SetFocus(true);
        for (auto [deg, _] : delegates()) { down_cast<Delegate>(deg)->DidBtnFocus(this, btn); }
    }
}

}  // namespace ui

}  // namespace nyaa