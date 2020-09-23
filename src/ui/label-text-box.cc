#include "ui/label-text-box.h"
#include "game/game.h"
#include "resource/font-library.h"
#include "base/slice.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

namespace ui {

LabelTextBox::LabelTextBox(Id id, std::string_view name, Component *parent) : Component(id, name, parent) {}
LabelTextBox::LabelTextBox(Id id, res::TextID name, Component *parent) : Component(id, name, parent) {}
LabelTextBox::~LabelTextBox() {}

void LabelTextBox::OnPaint(double delta) {
    // TODO:
}

}  // namespace ui

}  // namespace nyaa
