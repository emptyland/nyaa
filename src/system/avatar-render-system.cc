#include "system/avatar-render-system.h"
#include "component/avatar-component.h"
#include "component/movement-component.h"
#include "resource/texture-library.h"
#include "resource/avatar-library.h"
#include "resource/shader-library.h"
#include "game/game.h"
#include "game/constants.h"
#include <GL/glew.h>
#include <math.h>

namespace nyaa {

namespace sys {

const float AvatarRenderSystem::kVertices[] = {
    // :front
    -1, -1, -1, /*normal*/ 0, -1, 0, /*uv*/ 0, 0,  // :format
    -1, -1, +1, /*normal*/ 0, -1, 0, /*uv*/ 0, 0,  // :format
    +1, -1, +1, /*normal*/ 0, -1, 0, /*uv*/ 0, 0,  // :format
    +1, -1, -1, /*normal*/ 0, -1, 0, /*uv*/ 0, 0,  // :format
    // :back
    -1, +1, -1, /*normal*/ 0, +1, 0, /*uv*/ 0, 0,  // :format
    +1, +1, -1, /*normal*/ 0, +1, 0, /*uv*/ 0, 0,  // :format
    +1, +1, +1, /*normal*/ 0, +1, 0, /*uv*/ 0, 0,  // :format
    -1, +1, +1, /*normal*/ 0, +1, 0, /*uv*/ 0, 0,  // :format
};

void AvatarRenderSystem::Prepare(res::AvatarLibrary *lib) {
    DCHECK(!initialized_);
    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    std::vector<GLfloat> buf;
    for (const auto &pair : lib->resource()) {
        res::Avatar *def = pair.second;
        def->set_vbo_hint(static_cast<int>(buf.size() / 8));

        for (int i = res::Avatar::kUp; i < res::Avatar::kMaxDir; i++) {
            for (int j = 0; j < def->frames_count(); j++) {
                size_t pos = buf.size();
                buf.resize(buf.size() + 4 * 8);
                GLfloat *vectices = &buf[pos];

                res::Texture *frame = def->frame(static_cast<res::Avatar::Direction>(i), j);

                vectices[0] = 0;
                vectices[1] = 0;
                vectices[2] = 0;

                vectices[8]  = 0;
                vectices[9]  = 0;
                vectices[10] = 1 * frame->aspect_ratio();

                vectices[16] = 1;
                vectices[17] = 0;
                vectices[18] = 1 * frame->aspect_ratio();

                vectices[24] = 1;
                vectices[25] = 0;
                vectices[26] = 0;

                for (int k = 0; k < 4; k++) {
                    vectices[k * 8 + 3] = 0;
                    vectices[k * 8 + 4] = -1;
                    vectices[k * 8 + 5] = 0;

                    vectices[k * 8 + 6] = frame->coord((k + 3) % 4).x;
                    vectices[k * 8 + 7] = frame->coord((k + 3) % 4).y;
                }
            }
        }
    }
    glBufferData(GL_ARRAY_BUFFER, buf.size() * sizeof(GLfloat), &buf[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    initialized_ = true;
}

void AvatarRenderSystem::Render(com::MovementComponent *movement, com::AvatarComponent *avatar, double delta) {
    if (movement->is_horizontal_stop()) {
        // avatar->set_time(0);
        avatar->set_speed(0);
    } else {
        avatar->set_speed(movement->NormalHorizontalSpeed() * 1.2);  // TODO

        // [-π, π]
        // -π/2: North (Up)
        //    0: East  (Right)
        //  π/2: South (Down)
        //    π: West  (Left)
        float dir = movement->GetMovingDirection();

        if (dir >= -M_PI_4 * 3 && dir < -M_PI_4) {  // [-3π/4, -π/4)
            avatar->set_dir(res::Avatar::kDown);
        } else if (dir >= -M_PI_4 && dir < M_PI_4) {  // [-π/4, π/4)
            avatar->set_dir(res::Avatar::kRight);
        } else if (dir >= M_PI_4 && dir < M_PI_4 * 3) {  // [π/4, 3π/4)
            avatar->set_dir(res::Avatar::kUp);
        } else {  // [3π/4, )
            avatar->set_dir(res::Avatar::kLeft);
        }
    }
    int frame = avatar->FrameIndex(delta);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, avatar->GetFrame()->tex_id());

    res::BlockShaderProgram *shader = Game::This()->shader_lib()->block_program();

    float z = movement->coord().z - kTerrainSurfaceLevel;

    Matrix<float> mat;
    mat.Translate(0, 0, z - 0.5);  // TODO:
    shader->SetModelMatrix(mat);
    shader->SetSpecularMaterial({0.9, 0.9, 0.7});

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    shader->Enable();
    shader->SetPositionAttribute(4, 8, 0);
    shader->SetNormalAttribute(4, 8, 3);
    shader->SetUVAttribute(4, 8, 6);
    glDrawArrays(GL_QUADS, avatar->def()->vbo_hint() + frame * 4, 4);
    //DLOG(INFO) << "frame: " << frame;
    shader->Disable();
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

}  // namespace sys

}  // namespace nyaa