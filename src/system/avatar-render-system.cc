#include "system/avatar-render-system.h"
#include "component/avatar-component.h"
#include "component/movement-component.h"
#include "resource/texture-library.h"
#include "resource/avatar-library.h"
#include "game/game.h"
#include "game/constants.h"
#include <GL/glew.h>
#include <math.h>

namespace nyaa {

namespace sys {

void AvatarRenderSystem::Prepare(res::AvatarLibrary *avatar_lib) {
    DCHECK(!initialized_);
    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    std::vector<GLfloat> buf;
    for (const auto &pair : avatar_lib->avatars()) {
        res::Avatar *def = pair.second;

        // TODO:
    }
    glBufferData(GL_ARRAY_BUFFER, buf.size() * sizeof(GLfloat), &buf[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    initialized_ = true;
}

void AvatarRenderSystem::Render(com::MovementComponent *movement, com::AvatarComponent *avatar, double delta) {
    const Vector2i fb_size{Game::This()->fb_w(), Game::This()->fb_h()};

    if (movement->is_horizontal_stop()) {
        // avatar->set_time(0);
        avatar->set_speed(0);
    } else {
        avatar->set_speed(0.9);  // TODO

        // [-π, π]
        // -π/2: North (Up)
        //    0: East  (Right)
        //  π/2: South (Down)
        //    π: West  (Left)
        float dir = movement->GetMovingDirection();

        if (dir >= -M_PI_4 * 3 && dir < -M_PI_4) {  // [-3π/4, -π/4)
            avatar->set_dir(res::Avatar::kUp);
        } else if (dir >= -M_PI_4 && dir < M_PI_4) {  // [-π/4, π/4)
            avatar->set_dir(res::Avatar::kRight);
        } else if (dir >= M_PI_4 && dir < M_PI_4 * 3) {  // [π/4, 3π/4)
            avatar->set_dir(res::Avatar::kDown);
        } else {  // [3π/4, )
            avatar->set_dir(res::Avatar::kLeft);
        }
    }
    avatar->AddTime(delta);
    res::Texture *frame = avatar->GetFrame();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, frame->tex_id());
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);

    float dy = movement->coord().z - kTerrainSurfaceLevel;

    glTexCoord2f(frame->coord(0).x, frame->coord(0).y);
    glVertex3f(0, 2 + dy, 0);

    glTexCoord2f(frame->coord(1).x, frame->coord(1).y);
    glVertex3f(1.5, 2 + dy, 0);

    glTexCoord2f(frame->coord(2).x, frame->coord(2).y);
    glVertex3f(1.5, 0 + dy, 0);

    glTexCoord2f(frame->coord(3).x, frame->coord(3).y);
    glVertex3f(0, 0 + dy, 0);

    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

}  // namespace sys

}  // namespace nyaa