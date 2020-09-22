#include "ui/avatar-selector.h"
#include "resource/avatar-library.h"
#include "resource/texture-library.h"
#include "game/game.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

namespace ui {

AvatarSelector::AvatarSelector(Id id, Component *parent) : Component(id, parent) {}

AvatarSelector::~AvatarSelector() {}

void AvatarSelector::HandleMouseButtonInput(int button, int action, int mods, bool *should_break) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        switch (cursor_) {
            case kNext:
                for (auto [deg, _] : *mutable_delegates()) { down_cast<Delegate>(deg)->OnNextAvatar(this); }
                *should_break = true;
                break;
            case kPrev:
                for (auto [deg, _] : *mutable_delegates()) { down_cast<Delegate>(deg)->OnPrevAvatar(this); }
                *should_break = true;
                break;

            default: break;
        }
    }
}

void AvatarSelector::OnMouseMove(double x, double y) {
    cursor_ = kNone;
    if (!InBound<int>(bound(), x, y)) { return; }

    int cell = bound().w / 3;
    if (x >= bound().x && x <= bound().x + cell) {
        cursor_ = kPrev;
    } else if (x >= bound().x + 2 * cell && x < bound().x + bound().w) {
        cursor_ = kNext;
    }
}

void AvatarSelector::OnPaint(double delta) {
    int cell = bound().w / 3;

    res::Avatar *def = nullptr;
    if (data_) { data_->OnAvatarProduce(this, &def); }
    if (def) {
        res::Texture *tex = def->key_frame(res::Avatar::kDown);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex->tex_id());

        int w = cell / tex->aspect_ratio();
        int x = bound().x + cell + (cell - w) / 2;

        glBegin(GL_QUADS);
        glColor3f(1, 1, 1);
        glTexCoord2f(tex->coord(3).x, tex->coord(3).y);
        glVertex2i(x, bound().y);
        glTexCoord2f(tex->coord(0).x, tex->coord(0).y);
        glVertex2i(x, bound().y + bound().h);
        glTexCoord2f(tex->coord(1).x, tex->coord(1).y);
        glVertex2i(x + w, bound().y + bound().h);
        glTexCoord2f(tex->coord(2).x, tex->coord(2).y);
        glVertex2i(x + w, bound().y);
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }

    glBegin(GL_LINE_LOOP);
    glColor4f(kBorderColor.x, kBorderColor.y, kBorderColor.z, kBorderColor.w);
    glVertex2i(bound().x + cell, bound().y);
    glVertex2i(bound().x + cell, bound().y + bound().h);
    glVertex2i(bound().x + 2 * cell, bound().y + bound().h);
    glVertex2i(bound().x + 2 * cell, bound().y);

    glVertex2i(bound().x + cell + 2, bound().y + 2);
    glVertex2i(bound().x + cell + 2, bound().y + bound().h - 4);
    glVertex2i(bound().x + 2 * cell - 4, bound().y + bound().h - 4);
    glVertex2i(bound().x + 2 * cell - 4, bound().y + 2);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(1, 1, 1);
    glVertex2i(bound().x + cell - 16, bound().y + bound().h / 6);
    glVertex2i(bound().x + cell - 16, bound().y + bound().h * 5 / 6);
    glVertex2i(bound().x + cell / 2, bound().y + bound().h / 2);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(1, 1, 1);
    glVertex2i(bound().x + 2 * cell + 16, bound().y + bound().h / 6);
    glVertex2i(bound().x + 2 * cell + 16, bound().y + bound().h * 5 / 6);
    glVertex2i(bound().x + 2 * cell + cell / 2, bound().y + bound().h / 2);
    glEnd();

    switch (cursor_) {
        case kPrev:
            glBegin(GL_POLYGON);
            glColor3f(1, 1, 1);
            glVertex2i(bound().x + cell - 20, bound().y + bound().h / 6 + 12);
            glVertex2i(bound().x + cell - 20, bound().y + bound().h * 5 / 6 - 12);
            glVertex2i(bound().x + cell / 2 + 8, bound().y + bound().h / 2);
            glEnd();
            break;
        case kNext:
            glBegin(GL_POLYGON);
            glColor3f(1, 1, 1);
            glVertex2i(bound().x + 2 * cell + 20, bound().y + bound().h / 6 + 12);
            glVertex2i(bound().x + 2 * cell + 20, bound().y + bound().h * 5 / 6 - 12);
            glVertex2i(bound().x + 2 * cell + cell / 2 - 8, bound().y + bound().h / 2);
            glEnd();
            break;

        default: break;
    }
}

}  // namespace ui

}  // namespace nyaa
