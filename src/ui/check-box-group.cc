#include "ui/check-box-group.h"
#include "resource/font-library.h"
#include "game/game.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

namespace ui {

CheckBoxGroup::CheckBoxGroup(Id id, std::string_view name, Component *parent) : Component(id, parent) {
    set_name(std::string(name.data(), name.size()));
}

CheckBoxGroup::CheckBoxGroup(Id id, res::TextID name, Component *parent) : Component(id, parent) { SetName(name); }

CheckBoxGroup::~CheckBoxGroup() {}

LabelCheckBox *CheckBoxGroup::AddCheckBox(Id id, std::string_view name) { (void)new LabelCheckBox(id, name, this); }

LabelCheckBox *CheckBoxGroup::AddCheckBox(Id id, res::TextID name) { (void)new LabelCheckBox(id, name, this); }

void CheckBoxGroup::OnPaint(double delta) {}

}  // namespace ui

}  // namespace nyaa