#include "system/sprite-render-system.h"
#include "component/sprite-component.h"
#include "component/plant-component.h"
#include "resource/texture-library.h"
#include "resource/shader-library.h"
#include "game/matrix.h"
#include "game/game.h"
#include <GL/glew.h>
#include <math.h>

namespace nyaa {

namespace sys {

void SpriteRenderSystem::Prepare(res::SpriteLibrary *lib) {
    DCHECK(!initialized_);
    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    std::vector<GLfloat> buf;
    for (const auto &pair : lib->resource()) {
        res::Sprite *def = pair.second;
        def->set_vbo_hint(static_cast<int>(buf.size() / 8));

        for (int i = 0; i < def->frames_count(); i++) {
            size_t pos = buf.size();
            buf.resize(buf.size() + 4 * 8);
            GLfloat *vectices = &buf[pos];

            res::Texture *frame = def->frame(i);

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

    glBufferData(GL_ARRAY_BUFFER, buf.size() * sizeof(GLfloat), &buf[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    initialized_ = true;
}

void SpriteRenderSystem::RenderPlant(const Vector3f &view, com::PlantComponent *plant, double delta) {
    Vector3f d = plant->position() - view;

    Matrix model;
    model.Translate(d.x, d.y, d.z + 0.5);
    Matrix mat;
    mat.Scale(3, 3, 3);
    model.Multiply(mat);

    res::BlockShaderProgram *shader = Game::This()->shader_lib()->block_program();
    shader->SetModelMatrix(model);

    int index = plant->IndexFrame(delta);
    Render(plant->sprite(), index, delta);
}

void SpriteRenderSystem::Render(res::Sprite *def, int frame_index, double delta) {
    res::BlockShaderProgram *shader = Game::This()->shader_lib()->block_program();
    shader->SetAmbientMaterial(def->ambient_meterial());
    shader->SetDiffuseMaterial(def->diffuse_meterial());
    shader->SetSpecularMaterial(def->specular_meterial());

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, def->frame(frame_index)->tex_id());

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    shader->Enable();
    shader->SetPositionAttribute(4, 8, 0);
    shader->SetNormalAttribute(4, 8, 3);
    shader->SetUVAttribute(4, 8, 6);
    glDrawArrays(GL_QUADS, def->vbo_hint() + frame_index * 4, 4);
    // DLOG(INFO) << "frame: " << frame;
    shader->Disable();
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

}  // namespace sys

}  // namespace nyaa
