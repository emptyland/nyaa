#include "ui/input-box.h"
#include "resource/font-library.h"
#include "game/game.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

namespace ui {

InputBox::InputBox(Id id, Component *parent /* = nullptr*/) : Component(id, parent) {}

InputBox::~InputBox() {}

std::string InputBox::Utf8Text() {
    std::string buf;
    for (char32_t codepoint : text_) {
        buf.append(1, static_cast<char>(codepoint & 0xff));
    }
    return buf;
}

void InputBox::HandleKeyEvent(bool *did) {
    if (TestKeyPress(GLFW_KEY_ENTER)) {
        for (auto [deg, _] : *mutable_delegates()) {
            // :format
            down_cast<Delegate>(deg)->DidEnter(this);
        }
        *did = true;
        return;
    }

    if (TestKeyPress(GLFW_KEY_BACKSPACE)) {
        if (DeltaTest(0.1) && cursor_ > 0) {
            text_.erase(text_.begin() + cursor_ - 1);
            cursor_--;
        }
        *did = true;
        return;
    }

    // if (TestKeyPress(GLFW_KEY_LEFT)) {
    //     if (cursor_ > 0) { cursor_--; }
    //     *did = true;
    //     return;
    // }

    // if (TestKeyPress(GLFW_KEY_RIGHT)) {
    //     if (cursor_ < text_.size()) { cursor_++; }
    //     *did = true;
    //     return;
    // }
}

void InputBox::HandleCharInput(char32_t code, bool *did) {
    // DLOG(INFO) << "char input: " << code << " cursor: " << cursor_;
    text_.insert(text_.begin() + cursor_, code);
    cursor_++;
    *did = true;
}

void InputBox::DidFocus(bool focus) {
    DLOG(INFO) << "focus: " << focus;
    for (auto [deg, _] : *mutable_delegates()) { deg->DidForce(this); }
}

void InputBox::OnPaint(double delta) {
    if (!font()) { set_font(Game::This()->font_lib()->default_face()); }

    DrawBorder(delta);

    std::vector<float> vertices;

    Boundf rect =
        font()->Render(Vec3(bound().x + 2, bound().y + 2 + font_bearing(), 0), font_scale(), text_, &vertices);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, font()->buffered_tex());

    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 1.0);
    for (int i = 0; i < vertices.size(); i += 5) {
        glTexCoord2f(vertices[i + 3], vertices[i + 4]);
        glVertex3f(vertices[i + 0], vertices[i + 1], vertices[i + 2]);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);

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
