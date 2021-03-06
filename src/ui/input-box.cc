#include "ui/input-box.h"
#include "resource/font-library.h"
#include "game/game.h"
#include "base/slice.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

namespace ui {

InputBox::InputBox(Id id, Component *parent /* = nullptr*/) : Component(id, parent) {}

InputBox::~InputBox() {}

std::string InputBox::Utf8Text() {
    std::string buf;
    base::Slice::ConvertUTF32ToUTF8(text_, &buf);
    return buf;
}

void InputBox::SetUtf8Text(std::string_view text) {
    base::CodePointIteratorUtf8 iter(text);
    text_.clear();
    text_.reserve(text.size());
    for (iter.SeekFirst(); iter.Valid(); iter.Next()) { text_.append(1, *iter); }
    cursor_ = static_cast<int>(text_.size());
}

void InputBox::HandleKeyInput(int key, int code, int action, int mods, bool *should_break) {
    switch (key) {
        case GLFW_KEY_ENTER:
            for (auto [deg, _] : *mutable_delegates()) {
                // :format
                down_cast<Delegate>(deg)->DidEnter(this);
            }
            *should_break = true;
            break;
        case GLFW_KEY_BACKSPACE:
            if (cursor_ > 0) {
                text_.erase(text_.begin() + cursor_ - 1);
                cursor_--;
            }
            *should_break = true;
            break;

        default: break;
    }
}

void InputBox::HandleCharInput(char32_t code, bool *should_break) {
    if (number_only() && (code < '0' || code > '9')) { return; }
    text_.insert(text_.begin() + cursor_, code);
    cursor_++;
    *should_break = true;
}

void InputBox::DidFocus(bool focus) {
    for (auto [deg, _] : *mutable_delegates()) { deg->DidForce(this); }
}

void InputBox::OnPaint(double delta) {
    DrawBorder(kBorderColor, delta);

    std::vector<float> vertices;

    Boundf rect = DrawLabel(Vec3(bound().x + 2, bound().y + 2 + font_bearing(), 0), Vec3(kFontColor), text_);

    time_ += delta;
    uint64_t mills = time_ * 1000;
    if (IsFocus() && mills % 1000 > 500) {
        rect.x += rect.w;
        rect.y = bound().y + 4;
        rect.h = bound().h - 8;
        glBegin(GL_QUADS);
        glColor3f(1, 1, 1);
        glVertex2f(rect.x, rect.y);
        glVertex2f(rect.x + 4, rect.y);
        glVertex2f(rect.x + 4, rect.y + rect.h);
        glVertex2f(rect.x, rect.y + rect.h);
        glEnd();
    }
}

}  // namespace ui

}  // namespace nyaa
