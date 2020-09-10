#include "ui/button-group.h"
#include "resource/font-library.h"
#include "game/game.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

namespace ui {

ButtonGroup::ButtonGroup(Id id, Component *parent)
    : Component(id, parent), font_(Game::This()->font_lib()->default_face()) {}

ButtonGroup::~ButtonGroup() {
    for (auto row : rows_) { delete[] row; }
}

ButtonGroup::Button *ButtonGroup::AddButton(std::string_view name, Id id, int column) {
    DCHECK_GE(column, 0);
    DCHECK_LT(column, column_count_);
    Button *btn = nullptr;
    for (auto row : rows_) {
        if (!row[column].IsUsed()) {
            btn = &row[column];
            break;
        }
    }
    if (!btn) {
        rows_.push_back(new Button[column_count_]);
        btn = &rows_.back()[column];
    }

    btn->SetUsed(true);
    btn->SetEnable(true);
    btn->name_         = name;
    btn->id_           = id;
    id_to_buttons_[id] = btn;
    return btn;
}

void ButtonGroup::HandleKeyEvent(bool *did) {}

void ButtonGroup::HandleMouseEvent(double x, double y, bool *did) {}

void ButtonGroup::OnMouseMove(double x, double y) {}

void ButtonGroup::OnPaint(double delta) {
    if (!font()) { set_font(Game::This()->font_lib()->default_face()); }

    DrawBorder(delta);
}

}  // namespace ui

}  // namespace nyaa