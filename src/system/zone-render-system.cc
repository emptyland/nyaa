#include "system/zone-render-system.h"
#include "system/geometry-transform-system.h"
#include "component/zone-component.h"
#include "resource/texture-library.h"
#include "game/game.h"
#include <GL/glew.h>

namespace nyaa {

namespace sys {

struct VertexBundle {
    Vertex2f tex;
    Vertex3f nor;
    Vertex3f vec;
};

static const GLuint vertex_index[][4] = {
    0,  2,  3,  1,  8,  9,  10, 11,  // 0, 4, 6, 2
    12, 14, 15, 13,                  // 0, 1, 5, 4
    4,  5,  7,  6,  16, 18, 19, 17,  // 1, 3, 7, 5
    20, 22, 23, 21,                  // 2, 6, 7, 3
};

static const Vertex3f vertex_normal[] = {
    /* [0]  */ {0, 0, 1},   // back
    /* [1]  */ {-1, 0, 0},  // left
    /* [2]  */ {0, -1, 0},  // bottom
    /* [3]  */ {0, 0, -1},  // front
    /* [4]  */ {1, 0, 0},   // right
    /* [5]  */ {0, 1, 0},   // top
};

void ZoneRenderSystem::Render(com::ZoneComponent *zone) {
    const Vertex2i fb_size{Game::This()->fb_w(), Game::This()->fb_h()};

    glViewport(0, 0, fb_size.x, fb_size.y);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tile_tex_id());
    glColor3f(1.0, 1.0, 1.0);

    for (int j = 0; j < zone->viewport().bound().y; j++) {
        for (int i = 0; i < zone->viewport().bound().x; i++) {
            // :format
            RenderSurface(zone, i, j);
        }
    }

    glDisable(GL_BLEND);
}

// Vertex3i pos{-n_cubes, -1, n_cubes};
void ZoneRenderSystem::RenderSurface(com::ZoneComponent *zone, int i, int j) {
    VertexBundle vertexs[24];  // Texture + vertex
    Vertex3f     p0;
    p0.x = (-zone->viewport().bound().x / 2 + i - zone->viewport().adjust_center_x()) * cube_size_;
    p0.z = (zone->viewport().bound().y / 2 - j + zone->viewport().adjust_center_y()) * cube_size_;

    for (int z = kTerrainSurfaceLevel; z < kTerrainMaxLevels; z++) {
        com::CubeComponent *cube = zone->CubeAt(i, j, z);
        if (cube->IsTransparent()) { continue; }

        res::Cube *def = DCHECK_NOTNULL(Game::This()->cube_lib()->cube(cube->kind()));

        res::Texture *tt = def->top_tex();
        res::Texture *et = def->edge_tex();

        p0.y = (-1 + (z - kTerrainSurfaceLevel)) * cube_size_;

        if (cube->IsPlant()) {
            RenderPlant(def, p0, tt);
            continue;
        }

        Vertex3f points[8] = {p0, p0, p0, p0, p0, p0, p0, p0};
        points[1].x += cube_size_;
        points[2].y += cube_size_;
        points[3].x += cube_size_;
        points[3].y += cube_size_;
        points[4].z += cube_size_;
        points[5].x += cube_size_;
        points[5].z += cube_size_;
        points[6].y += cube_size_;
        points[6].z += cube_size_;
        points[7].x += cube_size_;
        points[7].y += cube_size_;
        points[7].z += cube_size_;

        // back
        vertexs[0].tex = et->coord(0);
        vertexs[0].nor = vertex_normal[0];
        vertexs[0].vec = points[0];
        vertexs[2].tex = et->coord(1);
        vertexs[2].nor = vertex_normal[0];
        vertexs[2].vec = points[2];
        vertexs[3].tex = et->coord(2);
        vertexs[3].nor = vertex_normal[0];
        vertexs[3].vec = points[3];
        vertexs[1].tex = et->coord(3);
        vertexs[1].nor = vertex_normal[0];
        vertexs[1].vec = points[1];

        // left
        vertexs[8].tex  = et->coord(0);
        vertexs[8].nor = vertex_normal[1];
        vertexs[8].vec  = points[0];
        vertexs[9].tex  = et->coord(1);
        vertexs[9].nor = vertex_normal[1];
        vertexs[9].vec  = points[4];
        vertexs[10].tex = et->coord(2);
        vertexs[10].nor = vertex_normal[1];
        vertexs[10].vec = points[6];
        vertexs[11].tex = et->coord(3);
        vertexs[11].nor = vertex_normal[1];
        vertexs[11].vec = points[2];

        // bottom
        vertexs[12].tex = et->coord(0);
        vertexs[12].nor = vertex_normal[2];
        vertexs[12].vec = points[0];
        vertexs[14].tex = et->coord(1);
        vertexs[14].nor = vertex_normal[2];
        vertexs[14].vec = points[1];
        vertexs[15].tex = et->coord(2);
        vertexs[15].nor = vertex_normal[2];
        vertexs[15].vec = points[5];
        vertexs[13].tex = et->coord(3);
        vertexs[13].nor = vertex_normal[2];
        vertexs[13].vec = points[4];

        // front
        vertexs[4].tex = et->coord(0);
        vertexs[4].nor = vertex_normal[3];
        vertexs[4].vec = points[4];
        vertexs[5].tex = et->coord(1);
        vertexs[5].nor = vertex_normal[3];
        vertexs[5].vec = points[5];
        vertexs[7].tex = et->coord(2);
        vertexs[7].nor = vertex_normal[3];
        vertexs[7].vec = points[7];
        vertexs[6].tex = et->coord(3);
        vertexs[6].nor = vertex_normal[3];
        vertexs[6].vec = points[6];

        // right
        vertexs[16].tex = et->coord(0);
        vertexs[16].nor = vertex_normal[4];
        vertexs[16].vec = points[1];
        vertexs[18].tex = et->coord(1);
        vertexs[18].nor = vertex_normal[4];
        vertexs[18].vec = points[3];
        vertexs[19].tex = et->coord(2);
        vertexs[19].nor = vertex_normal[4];
        vertexs[19].vec = points[7];
        vertexs[17].tex = et->coord(3);
        vertexs[17].nor = vertex_normal[4];
        vertexs[17].vec = points[5];

        // top
        vertexs[20].tex = tt->coord(0);
        vertexs[20].nor = vertex_normal[5];
        vertexs[20].vec = points[2];
        vertexs[22].tex = tt->coord(1);
        vertexs[22].nor = vertex_normal[5];
        vertexs[22].vec = points[6];
        vertexs[23].tex = tt->coord(2);
        vertexs[23].nor = vertex_normal[5];
        vertexs[23].vec = points[7];
        vertexs[21].tex = tt->coord(3);
        vertexs[21].nor = vertex_normal[5];
        vertexs[21].vec = points[3];

        glInterleavedArrays(GL_T2F_N3F_V3F, 0, vertexs);  // GL_T2F_N3F_V3F
        glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, vertex_index);
    }
}

void ZoneRenderSystem::RenderPlant(res::Cube *def, Vertex3f p0, res::Texture *tex) {
    glBindTexture(GL_TEXTURE_2D, tex->tex_id());
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);

    glTexCoord2f(tex->coord(0).x, tex->coord(0).y);
    glVertex3f(p0.x, p0.y + def->vh() * cube_size_ * tex->aspect_ratio(), p0.z + 0.5 * cube_size_);

    glTexCoord2f(tex->coord(1).x, tex->coord(1).y);
    glVertex3f(p0.x + 1.5 * cube_size_, p0.y + def->vh() * cube_size_ * tex->aspect_ratio(), p0.z + 0.5 * cube_size_);

    glTexCoord2f(tex->coord(2).x, tex->coord(2).y);
    glVertex3f(p0.x + 1.5 * cube_size_, p0.y, p0.z + 0.5 * cube_size_);

    glTexCoord2f(tex->coord(3).x, tex->coord(3).y);
    glVertex3f(p0.x, p0.y, p0.z + 0.5 * cube_size_);

    glEnd();

    // Restore
    glBindTexture(GL_TEXTURE_2D, tile_tex_id());
    glColor3f(1.0, 1.0, 1.0);
}

}  // namespace sys

}  // namespace nyaa
