#include "ui/property-box.h"

namespace nyaa {

namespace ui {

PropertyBox::PropertyBox(Id id, Component *parent) : Component(id, parent) {}
PropertyBox::~PropertyBox() {}

void PropertyBox::HandleMouseButtonInput(int button, int action, int mods, bool *should_break) {}
void PropertyBox::OnMouseMove(double x, double y) {}
void PropertyBox::OnPaint(double delta) {}

}  // namespace ui

}  // namespace nyaa