#include "system/zone-render-system.h"
#include "system/geometry-transform-system.h"
#include "component/zone-component.h"
#include "resource/shader-library.h"
#include "resource/texture-library.h"
#include "game/game.h"
#include <GL/glew.h>

namespace nyaa {

namespace sys {

static const float positions[6][4][3] = {
    // :format
    {{-1, -1, -1}, {-1, -1, +1}, {-1, +1, -1}, {-1, +1, +1}},
    // :format
    {{+1, -1, -1}, {+1, -1, +1}, {+1, +1, -1}, {+1, +1, +1}},
    // :format
    {{-1, +1, -1}, {-1, +1, +1}, {+1, +1, -1}, {+1, +1, +1}},
    // :format
    {{-1, -1, -1}, {-1, -1, +1}, {+1, -1, -1}, {+1, -1, +1}},
    // :format
    {{-1, -1, -1}, {-1, +1, -1}, {+1, -1, -1}, {+1, +1, -1}},
    // :format
    {{-1, -1, +1}, {-1, +1, +1}, {+1, -1, +1}, {+1, +1, +1}}};
static const float normals[6][3] = {
    // :format
    {-1, 0, 0},
    // :format
    {+1, 0, 0},
    // :format
    {0, +1, 0},
    // :format
    {0, -1, 0},
    // :format
    {0, 0, -1},
    // :format
    {0, 0, +1}};

#if 0
struct VertexBundle {
    Vector2f tex;
    Vector3f nor;
    Vector3f vec;
};

static const GLuint vertex_index[][4] = {
    0,  2,  3,  1,  8,  9,  10, 11,  // 0, 4, 6, 2
    12, 14, 15, 13,                  // 0, 1, 5, 4
    4,  5,  7,  6,  16, 18, 19, 17,  // 1, 3, 7, 5
    20, 22, 23, 21,                  // 2, 6, 7, 3
};

static const Vector3f vertex_normal[] = {
    /* [0]  */ {0, 0, 1},   // back
    /* [1]  */ {-1, 0, 0},  // left
    /* [2]  */ {0, -1, 0},  // bottom
    /* [3]  */ {0, 0, -1},  // front
    /* [4]  */ {1, 0, 0},   // right
    /* [5]  */ {0, 1, 0},   // top
};
#endif

void ZoneRenderSystem::Prepare() {
    res::BlockShaderProgram *program = Game::This()->shader_lib()->block_program();
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_), vertices_, GL_STATIC_DRAW);
    program->SetPositionAttribute(4, 8, 0);
    program->SetNormalAttribute(4, 8, 3);
    program->SetUVAttribute(4, 8, 6);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ZoneRenderSystem::RenderTerrain(com::ZoneComponent *zone) {
    //------------------------------------------------------------------------------------------------------------------
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    //------------------------------------------------------------------------------------------------------------------

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tile_tex_id());
    glColor3f(1.0, 1.0, 1.0);

    for (int j = 0; j < zone->viewport().bound().y; j++) {
        for (int i = 0; i < zone->viewport().bound().x; i++) {
            // :format
            RenderSurface(zone, i, j);
        }
    }

    glDisable(GL_CULL_FACE);
}

// Vertex3i pos{-n_cubes, -1, n_cubes};
void ZoneRenderSystem::RenderSurface(com::ZoneComponent *zone, int i, int j) {
    VertexBundle vertexs[24];  // Texture + vertex
    Vector3f     p0;
    p0.x = (-zone->viewport().bound().x / 2 + i - zone->viewport().adjust_center_x()) * cube_size_;
    p0.y = (zone->viewport().bound().y / 2 - j + zone->viewport().adjust_center_y()) * cube_size_;

    for (int z = kTerrainSurfaceLevel; z < kTerrainMaxLevels; z++) {
        com::CubeComponent *cube = zone->CubeAt(i, j, z);
        if (cube->IsTransparent()) { continue; }
        if (cube->IsPlant()) { continue; }

        res::Cube *def = DCHECK_NOTNULL(Game::This()->cube_lib()->cube(cube->kind()));

        p0.z = (-1 + (z - kTerrainSurfaceLevel)) * cube_size_;

        glBindVertexArray(vao_);
        glDrawArrays(GL_QUADS, 0, 24);
        glBindVertexArray(0);
    }
}

void ZoneRenderSystem::MakeCube(const Vector3f &p0) {

}

}  // namespace sys

}  // namespace nyaa
