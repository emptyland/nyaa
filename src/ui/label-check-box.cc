#include "ui/label-check-box.h"

namespace nyaa {

namespace ui {

LabelCheckBox::LabelCheckBox(Id id, std::string_view name, Component *parent /* = nullptr*/) : Component(id, parent) {
    set_name(std::string(name.data(), name.size()));
}

LabelCheckBox::~LabelCheckBox() {}

void LabelCheckBox::HandleMouseButtonInput(int button, int action, int mods, bool *should_break) {
    // TODO:
}

void LabelCheckBox::OnMouseMove(double x, double y) {
    // TODO:
}

void LabelCheckBox::OnPaint(double delta) {
    // TODO:
}

}  // namespace ui

}  // namespace nyaa