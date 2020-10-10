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

const float ZoneRenderSystem::kVertices[] = {
    // top
    0, 0, 1, /*normal*/ 0, 0, 1, /*uv*/ 0, 0,  // :format
    0, 1, 1, /*normal*/ 0, 0, 1, /*uv*/ 0, 0,  // :format
    1, 1, 1, /*normal*/ 0, 0, 1, /*uv*/ 0, 0,  // :format
    1, 0, 1, /*normal*/ 0, 0, 1, /*uv*/ 0, 0,  // :format
    // bottom
    0, 0, 0, /*normal*/ 0, 0, -1, /*uv*/ 0, 0,  // :format
    0, 1, 0, /*normal*/ 0, 0, -1, /*uv*/ 0, 0,  // :format
    1, 1, 0, /*normal*/ 0, 0, -1, /*uv*/ 0, 0,  // :format
    1, 0, 0, /*normal*/ 0, 0, -1, /*uv*/ 0, 0,  // :format
    // :front
    0, 0, 0, /*normal*/ 0, -1, 0, /*uv*/ 0, 0,  // :format
    0, 0, 1, /*normal*/ 0, -1, 0, /*uv*/ 0, 0,  // :format
    1, 0, 1, /*normal*/ 0, -1, 0, /*uv*/ 0, 0,  // :format
    1, 0, 0, /*normal*/ 0, -1, 0, /*uv*/ 0, 0,  // :format
    // :back
    0, 1, 0, /*normal*/ 0, 1, 0, /*uv*/ 0, 0,  // :format
    1, 1, 0, /*normal*/ 0, 1, 0, /*uv*/ 0, 0,  // :format
    1, 1, 1, /*normal*/ 0, 1, 0, /*uv*/ 0, 0,  // :format
    0, 1, 1, /*normal*/ 0, 1, 0, /*uv*/ 0, 0,  // :format
    // :left
    0, 0, 0, /*normal*/ -1, 0, 0, /*uv*/ 0, 0,  // :format
    0, 1, 0, /*normal*/ -1, 0, 0, /*uv*/ 0, 0,  // :format
    0, 1, 1, /*normal*/ -1, 0, 0, /*uv*/ 0, 0,  // :format
    0, 0, 1, /*normal*/ -1, 0, 0, /*uv*/ 0, 0,  // :format
    // :right
    1, 0, 0, /*normal*/ 1, 0, 0, /*uv*/ 0, 0,  // :format
    1, 0, 1, /*normal*/ 1, 0, 0, /*uv*/ 0, 0,  // :format
    1, 1, 1, /*normal*/ 1, 0, 0, /*uv*/ 0, 0,  // :format
    1, 1, 0, /*normal*/ 1, 0, 0, /*uv*/ 0, 0,  // :format
};

ZoneRenderSystem::ZoneRenderSystem() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            vbo_[i][j].coord  = {-1, -1};
            vbo_[i][j].buffer = 0;
            vbo_[i][j].count  = 0;
        }
    }
}

void ZoneRenderSystem::Reset() {
    if (vbo_[1][1].buffer) {
        glDeleteBuffers(1, &vbo_[1][1].buffer);
        vbo_[1][1].buffer = 0;
    }
}

void ZoneRenderSystem::RenderTerrain(com::ZoneComponent *zone) {
    if (!vbo_[1][1].buffer) { GenBuffer(zone->mutable_region(), 1, 1); }

    //------------------------------------------------------------------------------------------------------------------
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    //------------------------------------------------------------------------------------------------------------------

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tile_tex_id());

    res::BlockShaderProgram *shader = Game::This()->shader_lib()->block_program();

    Matrix<float> mat;
    mat.Translate(-zone->viewport().center_coord().x, -zone->viewport().center_coord().y, 0);
    shader->SetModelMatrix(mat);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_[1][1].buffer);
    shader->Enable();
    glDrawArrays(GL_QUADS, 0, vbo_[1][1].count);
    shader->Disable();
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisable(GL_CULL_FACE);
}

void ZoneRenderSystem::GenBuffer(com::RegionComponent *region, int i, int j) {
    glDeleteBuffers(1, &vbo_[i][j].buffer);
    glGenBuffers(1, &vbo_[i][j].buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_[i][j].buffer);

    std::vector<GLfloat> buf;
    Vector3f             p0{0, 0, 0};
    for (int y = 0; y < kRegionSize; y++) {
        for (int x = 0; x < kRegionSize; x++) {
            p0.x = x * cube_size_;
            p0.y = y * cube_size_;

            for (int z = kTerrainSurfaceLevel; z < kTerrainMaxLevels; z++) {
                com::CubeComponent *cube = &region->floor(z)->cubes[x][y];
                if (cube->IsTransparent()) { continue; }
                if (cube->IsPlant()) { continue; }

                res::Cube *def = DCHECK_NOTNULL(Game::This()->cube_lib()->cube(cube->kind()));

                p0.z = (-1 + (z - kTerrainSurfaceLevel)) * cube_size_;
                MakeCube(def, p0, z == kTerrainSurfaceLevel, &buf);
            }
        }
    }
    glBufferData(GL_ARRAY_BUFFER, buf.size() * sizeof(float), &buf[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    vbo_[i][j].count = buf.size() / 8;
    vbo_[i][j].coord = region->global_coord();
}

void ZoneRenderSystem::MakeCube(const res::Cube *cube, const Vector3f &p0, bool surface, std::vector<float> *buf) {
    // float half_size  = cube_size_ / 2;
    int n_vertices = surface ? 4 : 24;

    size_t pos = buf->size();
    buf->resize(pos + n_vertices * 8);
    float *vertices = &(*buf)[pos];
    for (int i = 0; i < n_vertices; i++) {
        vertices[i * 8 + 0] = p0.x + kVertices[i * 8 + 0] * cube_size_;
        vertices[i * 8 + 1] = p0.y + kVertices[i * 8 + 1] * cube_size_;
        vertices[i * 8 + 2] = p0.z + kVertices[i * 8 + 2] * cube_size_;

        vertices[i * 8 + 3] = kVertices[i * 8 + 3];
        vertices[i * 8 + 4] = kVertices[i * 8 + 4];
        vertices[i * 8 + 5] = kVertices[i * 8 + 5];
    }
    for (int i = 0; i < 4; i++) {
        vertices[i * 8 + 6] = cube->top_tex()->coord(i).x;
        vertices[i * 8 + 7] = cube->top_tex()->coord(i).y;
    }
    if (!surface) {
        for (int i = 4; i < 24; i++) {
            vertices[i * 8 + 6] = cube->edge_tex()->coord(i % 4).x;
            vertices[i * 8 + 7] = cube->edge_tex()->coord(i % 4).y;
        }
    }

    // int n_vertices = 24;

    // size_t pos = buf->size();
    // buf->resize(pos + n_vertices * 8);
    // float *vertices = &(*buf)[pos];
    // for (int i = 0; i < n_vertices; i++) {
    //     vertices[i * 8 + 0] = p0.x + kVertices[i * 8 + 0] * cube_size_;
    //     vertices[i * 8 + 1] = p0.y + kVertices[i * 8 + 1] * cube_size_;
    //     vertices[i * 8 + 2] = p0.z + kVertices[i * 8 + 2] * cube_size_;

    //     vertices[i * 8 + 3] = kVertices[i * 8 + 3];
    //     vertices[i * 8 + 4] = kVertices[i * 8 + 4];
    //     vertices[i * 8 + 5] = kVertices[i * 8 + 5];
    // }
    // for (int i = 0; i < n_vertices; i++) {
    //     vertices[i * 8 + 6] = cube->top_tex()->coord(i % 4).x;
    //     vertices[i * 8 + 7] = cube->top_tex()->coord(i % 4).y;
    // }
}

}  // namespace sys

}  // namespace nyaa
