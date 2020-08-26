#include "system/zone-render-system.h"
#include "system/geometry-transform-system.h"
#include "component/zone-component.h"
#include "resource/shader-library.h"
#include "resource/texture-library.h"
#include "game/game.h"
#include "game/matrix.h"
#include <GL/glew.h>

namespace nyaa {

namespace sys {

static const float kVertices[] = {
    // top
    -1, -1, +1, /*normal*/ 0, 0, +1, /*uv*/ 0, 0,  // :format
    -1, +1, +1, /*normal*/ 0, 0, +1, /*uv*/ 0, 0,  // :format
    +1, +1, +1, /*normal*/ 0, 0, +1, /*uv*/ 0, 0,  // :format
    +1, -1, +1, /*normal*/ 0, 0, +1, /*uv*/ 0, 0,  // :format
    // bottom
    -1, -1, -1, /*normal*/ 0, 0, -1, /*uv*/ 0, 0,  // :format
    -1, +1, -1, /*normal*/ 0, 0, -1, /*uv*/ 0, 0,  // :format
    +1, +1, -1, /*normal*/ 0, 0, -1, /*uv*/ 0, 0,  // :format
    +1, -1, -1, /*normal*/ 0, 0, -1, /*uv*/ 0, 0,  // :format
    // :front
    -1, -1, -1, /*normal*/ 0, -1, 0, /*uv*/ 0, 0,  // :format
    +1, -1, -1, /*normal*/ 0, -1, 0, /*uv*/ 0, 0,  // :format
    +1, -1, +1, /*normal*/ 0, -1, 0, /*uv*/ 0, 0,  // :format
    -1, -1, +1, /*normal*/ 0, -1, 0, /*uv*/ 0, 0,  // :format
    // :back
    -1, +1, -1, /*normal*/ 0, +1, 0, /*uv*/ 0, 0,  // :format
    -1, +1, +1, /*normal*/ 0, +1, 0, /*uv*/ 0, 0,  // :format
    +1, +1, +1, /*normal*/ 0, +1, 0, /*uv*/ 0, 0,  // :format
    +1, +1, -1, /*normal*/ 0, +1, 0, /*uv*/ 0, 0,  // :format
    // :left
    -1, -1, -1, /*normal*/ -1, 0, 0, /*uv*/ 0, 0,  // :format
    -1, -1, +1, /*normal*/ -1, 0, 0, /*uv*/ 0, 0,  // :format
    -1, +1, +1, /*normal*/ -1, 0, 0, /*uv*/ 0, 0,  // :format
    -1, +1, -1, /*normal*/ -1, 0, 0, /*uv*/ 0, 0,  // :format
    // :right
    +1, -1, -1, /*normal*/ +1, 0, 0, /*uv*/ 0, 0,  // :format
    +1, -1, +1, /*normal*/ +1, 0, 0, /*uv*/ 0, 0,  // :format
    +1, +1, +1, /*normal*/ +1, 0, 0, /*uv*/ 0, 0,  // :format
    +1, +1, -1, /*normal*/ +1, 0, 0, /*uv*/ 0, 0,  // :format
};

ZoneRenderSystem::ZoneRenderSystem() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            vbo_[i][j].coord  = {-1, -1};
            vbo_[i][j].buffer = 0;
            vbo_[i][j].count = 0;
        }
    }
}

void ZoneRenderSystem::Prepare() {}

void ZoneRenderSystem::RenderTerrain(com::ZoneComponent *zone) {
    if (!zone->region()->GlobalCoordEqual(vbo_[1][1].coord.x, vbo_[1][1].coord.y)) {
        GenBuffer(zone->region(), 1, 1);
    }

    //------------------------------------------------------------------------------------------------------------------
    // glFrontFace(GL_CW);
    // glCullFace(GL_BACK);
    // glEnable(GL_CULL_FACE);
    //------------------------------------------------------------------------------------------------------------------

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tile_tex_id());

    res::BlockShaderProgram *shader = Game::This()->shader_lib()->block_program();
    Matrix<float> mat;
    mat.Translate(-zone->viewport().center_coord().x, -zone->viewport().center_coord().y, 0);
    shader->SetModelMatrix(mat);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_[1][1].buffer);
    shader->Enable();
    shader->SetPositionAttribute(4, 8, 0);
    shader->SetNormalAttribute(4, 8, 3);
    shader->SetUVAttribute(4, 8, 6);
    glDrawArrays(GL_QUADS, 0, vbo_[1][1].count);
    shader->Disable();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glDisable(GL_CULL_FACE);
}

void ZoneRenderSystem::GenBuffer(com::RegionComponent *region, int i, int j) {
    glDeleteBuffers(1, &vbo_[i][j].buffer);
    glGenBuffers(1, &vbo_[i][j].buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_[i][j].buffer);

    std::vector<GLfloat> buf;
    Vector3f             p0{0, 0, 0};
    for (int j = 0; j < kRegionSize; j++) {
        for (int i = 0; i < kRegionSize; i++) {
            p0.x = i * cube_size_;
            p0.y = j * cube_size_;

            for (int z = kTerrainSurfaceLevel; z < kTerrainMaxLevels; z++) {
                com::CubeComponent *cube = &region->floor(z)->cubes[i][j];
                if (cube->IsTransparent()) { continue; }
                if (cube->IsPlant()) { continue; }

                res::Cube *def = DCHECK_NOTNULL(Game::This()->cube_lib()->cube(cube->kind()));

                p0.z = (-1 + (z - kTerrainSurfaceLevel)) * cube_size_;
                MakeCube(def, p0, &buf);
            }
        }
    }
    glBufferData(GL_ARRAY_BUFFER, buf.size() * sizeof(float), &buf[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    vbo_[i][j].count = buf.size() / 8;
    vbo_[i][j].coord = region->global_coord();
}

void ZoneRenderSystem::MakeCube(const res::Cube *cube, const Vector3f &p0, std::vector<float> *buf) {
    float half_size = cube_size_ / 2;

    size_t pos = buf->size(); 
    buf->resize(pos + 24 * 8);
    float *vertices = &(*buf)[pos];
    for (int i = 0; i < 24; i++) {
        vertices[i * 8 + 0] = p0.x + kVertices[i * 8 + 0] * half_size;
        vertices[i * 8 + 1] = p0.y + kVertices[i * 8 + 1] * half_size;
        vertices[i * 8 + 2] = p0.z + kVertices[i * 8 + 2] * half_size;

        vertices[i * 8 + 3] = p0.x + kVertices[i * 8 + 3];
        vertices[i * 8 + 4] = p0.y + kVertices[i * 8 + 4];
        vertices[i * 8 + 5] = p0.z + kVertices[i * 8 + 5];
    }
    for (int i = 0; i < 4; i++) {
        vertices[i * 8 + 6] = cube->top_tex()->coord(i).x;
        vertices[i * 8 + 7] = cube->top_tex()->coord(i).y;
    }
    for (int i = 4; i < 24; i++) {
        vertices[i * 8 + 6] = cube->edge_tex()->coord(i % 4).x;
        vertices[i * 8 + 7] = cube->edge_tex()->coord(i % 4).y;
    }
}

#if 0
void ZoneRenderSystem::RenderSurface(com::ZoneComponent *zone, int i, int j) {
    Vector3f p0{0, 0, 0};
    p0.x = (-zone->viewport().bound().x / 2 + i - zone->viewport().adjust_center_x()) * cube_size_;
    p0.y = (zone->viewport().bound().y / 2 - j + zone->viewport().adjust_center_y()) * cube_size_;

    for (int z = kTerrainSurfaceLevel; z < kTerrainMaxLevels; z++) {
        com::CubeComponent *cube = zone->CubeAt(i, j, z);
        if (cube->IsTransparent()) { continue; }
        if (cube->IsPlant()) { continue; }

        res::Cube *def = DCHECK_NOTNULL(Game::This()->cube_lib()->cube(cube->kind()));

        p0.z = (-1 + (z - kTerrainSurfaceLevel)) * cube_size_;
        MakeCube(def, p0);
    }

    res::BlockShaderProgram *shader = Game::This()->shader_lib()->block_program();
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, buf_.size() * sizeof(float), &buf_[0], GL_STREAM_DRAW);
    shader->Enable();
    shader->SetPositionAttribute(4, 8, 0);
    shader->SetNormalAttribute(4, 8, 3);
    shader->SetUVAttribute(4, 8, 6);
    glDrawArrays(GL_QUADS, 0, buf_.size() / 8);
    shader->Disable();
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    buf_.clear();
}

void ZoneRenderSystem::MakeCube(const res::Cube *cube, const Vector3f &p0) {
    float half_size = cube_size_ / 2;

    size_t pos = buf_.size(); 
    buf_.resize(pos + 24 * 8);
    float *vertices = &buf_[pos];
    for (int i = 0; i < 24; i++) {
        vertices[i * 8 + 0] = p0.x + kVertices[i * 8 + 0] * half_size;
        vertices[i * 8 + 1] = p0.y + kVertices[i * 8 + 1] * half_size;
        vertices[i * 8 + 2] = p0.z + kVertices[i * 8 + 2] * half_size;
    }
    for (int i = 0; i < 4; i++) {
        vertices[i * 8 + 6] = cube->top_tex()->coord(i).x;
        vertices[i * 8 + 7] = cube->top_tex()->coord(i).y;
    }
    for (int i = 4; i < 24; i++) {
        vertices[i * 8 + 6] = cube->edge_tex()->coord(i % 4).x;
        vertices[i * 8 + 7] = cube->edge_tex()->coord(i % 4).y;
    }
}
#endif

}  // namespace sys

}  // namespace nyaa
