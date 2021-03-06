#include "ui/component.h"
#include "resource/text-library.h"
#include "resource/font-library.h"
#include "game/game.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

namespace ui {

Component::Component(Id id, std::string_view name, Component *parent)
    : next_(this)
    , prev_(this)
    , id_(id)
    , name_(name.data(), name.size())
    , font_(Game::This()->font_lib()->default_face())
    , parent_(parent) {
    if (parent_) { parent_->AddChild(this); }
}

Component::Component(Id id, res::TextID name, Component *parent)
    : Component(id, Game::This()->text_lib()->Load(name), parent) {}

/*virtual*/ ComponentDelegate::~ComponentDelegate() {}

/*virtual*/ void ComponentDelegate::DidForce(Component *sender) {
    // Ignore
}

/*virtual*/ void ComponentDelegate::OnCommand(Component *sender, Component::Id id) {
    // Ignore
}

Component::~Component() {
    for (auto [value, ownership] : delegates()) {
        if (ownership) { delete value; }
    }
    std::vector<const Component *> delay;
    for (auto child : children()) { delay.push_back(child); }
    for (auto child : delay) { delete child; }
}

void Component::SetName(res::TextID text) {
    std::string_view slice = Game::This()->text_lib()->Load(text);
    set_name(std::string(slice.data(), slice.size()));
}

bool Component::DeltaTest(double delta) {
    if (Game::This()->ts() - last_time() > delta) {
        set_last_time(Game::This()->ts());
        return true;
    }
    return false;
}

/*virtual*/ void Component::HandleMouseButtonInput(int button, int action, int mods, bool *should_break) {}

/*virtual*/ void Component::HandleKeyInput(int key, int code, int action, int mods, bool *should_break) {}

/*virtual*/ void Component::HandleCharInput(char32_t code, bool *should_break) {}

/*virtual*/ void Component::DidFocus(bool focus) {}

/*virtual*/ void Component::OnMouseMove(double x, double y) {}

/*static*/ Vector2i Component::GetCursorPosition() {
    double x = 0, y = 0;
    glfwGetCursorPos(Game::This()->window(), &x, &y);

    y = Game::This()->fb_h() - y * Game::This()->dpi_factor();
    x *= Game::This()->dpi_factor();
    return {static_cast<int>(x), static_cast<int>(y)};
}

bool Component::TestKeyPress(int key) { return glfwGetKey(Game::This()->window(), key) == GLFW_PRESS; }

void Component::DrawBorder(const Vector4f &color, double delta) {
    glBegin(GL_QUADS);
    glColor4fv(&bg_color().x);
    glVertex2f(bound().x, bound().y);
    glVertex2f(bound().x + bound().w, bound().y);
    glVertex2f(bound().x + bound().w, bound().y + bound().h);
    glVertex2f(bound().x, bound().y + bound().h);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor4fv(&color.x);
    glVertex2f(bound().x, bound().y);
    glVertex2f(bound().x + bound().w, bound().y);
    glVertex2f(bound().x + bound().w, bound().y + bound().h);
    glVertex2f(bound().x, bound().y + bound().h);

    glVertex2f(bound().x + 2, bound().y + 2);
    glVertex2f(bound().x + bound().w - 2, bound().y + 2);
    glVertex2f(bound().x + bound().w - 2, bound().y + bound().h - 2);
    glVertex2f(bound().x + 2, bound().y + bound().h - 2);
    glEnd();
}

Boundf Component::DrawLabel(const Vector3f &pos, const Vector3f &color, std::string_view label) {
    std::vector<float> vertices;

    Boundf rect = font()->Render(pos, font_scale(), label, &vertices);
    DrawText(color, vertices);
    return rect;
}

Boundf Component::DrawLabel(const Vector3f &pos, const Vector3f &color, std::u32string_view label) {
    std::vector<float> vertices;

    Boundf rect = font()->Render(pos, font_scale(), label, &vertices);
    DrawText(color, vertices);
    return rect;
}

void Component::DrawText(const Vector3f &color, const std::vector<float> &vertices) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, font()->buffered_tex());

    glBegin(GL_QUADS);
    glColor3fv(&color.x);
    for (int i = 0; i < vertices.size(); i += 5) {
        glTexCoord2f(vertices[i + 3], vertices[i + 4]);
        glVertex3f(vertices[i + 0], vertices[i + 1], vertices[i + 2]);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

}  // namespace ui

}  // namespace nyaa
