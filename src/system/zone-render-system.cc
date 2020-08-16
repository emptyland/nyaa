#include "system/zone-render-system.h"
#include "component/zone-component.h"
#include "resource/texture-library.h"
#include "game/game.h"
#include <GL/glew.h>

namespace nyaa {

namespace sys {

struct VertexBundle {
    Vertex2f tex;
    Vertex3f vec;
};

static const GLuint vertex_index[][4] = {
    0,  2,  3,  1,  8,  9,  10, 11,  // 0, 4, 6, 2
    12, 14, 15, 13,                  // 0, 1, 5, 4
    4,  5,  7,  6,  16, 18, 19, 17,  // 1, 3, 7, 5
    20, 22, 23, 21,                  // 2, 6, 7, 3
};

void ZoneRenderSystem::Render(com::ZoneComponent *zone) {
    const Vertex2i fb_size{Game::This()->fb_w(), Game::This()->fb_h()};

    glViewport(0, 0, fb_size.x, fb_size.y);

#if 0
    //------------------------------------------------------------------------------------------------------------------
    // Light
    GLfloat sun_light_position[] = {0.0f, 10.0f, 0.0f, 1.0f};  //光源的位置在世界坐标系圆心，齐次坐标形式
    GLfloat sun_light_ambient[]  = {0.1f, 0.1f, 0.1f, 1.0f};  // RGBA模式的环境光，为0
    GLfloat sun_light_diffuse[]  = {1.0f, 1.0f, 1.0f, 1.0f};  // RGBA模式的漫反射光，全白光
    GLfloat sun_light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};  // RGBA模式下的镜面光 ，全白光
    glLightfv(GL_LIGHT0, GL_POSITION, sun_light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, sun_light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, sun_light_specular);

    GLfloat sun_mat_ambient[]  = {0.0f, 0.0f, 0.0f, 1.0f};  //定义材质的环境光颜色，为0
    GLfloat sun_mat_diffuse[]  = {0.1f, 0.1f, 0.1f, 1.0f};  //定义材质的漫反射光颜色，为0
    GLfloat sun_mat_specular[] = {0.8f, 0.8f, 0.8f, 1.0f};  //定义材质的镜面反射光颜色，为0
    GLfloat sun_mat_emission[] = {0.8f, 0.8f, 0.8f, 1.0f};  //定义材质的辐射广颜色，为偏红色
    GLfloat sun_mat_shininess  = 0.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, sun_mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, sun_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, sun_mat_specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, sun_mat_emission);
    glMaterialf(GL_FRONT, GL_SHININESS, sun_mat_shininess);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    //------------------------------------------------------------------------------------------------------------------
#endif

    //------------------------------------------------------------------------------------------------------------------
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    //------------------------------------------------------------------------------------------------------------------

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glFrustum(0, fb_size.x, 0, fb_size.y, -100.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glPushMatrix();
    glRotated(rotate_angle_y(), -1, 0, 0);

    glPushMatrix();
    glRotated(rotate_angle_z(), 0.0, 1.0, 0.0);

    glPushMatrix();
    glScaled(scale(), scale(), scale());

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tile_tex_id());
    glColor3f(1.0, 1.0, 1.0);

    for (int j = 0; j < zone->viewport().bound().y; j++) {
        for (int i = 0; i < zone->viewport().bound().x; i++) {
            // :format
            RenderSurface(zone, i, j);
        }
    }

    glPopMatrix();  // glScaled
    glPopMatrix();  // glRotated
    glPopMatrix();  // glRotated

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    //------------------------------------------------------------------------------------------------------------------
    glDisable(GL_CULL_FACE);
    //------------------------------------------------------------------------------------------------------------------
}

// Vertex3i pos{-n_cubes, -1, n_cubes};
void ZoneRenderSystem::RenderSurface(com::ZoneComponent *zone, int i, int j) {
    VertexBundle vertexs[24];  // Texture + vertex
    Vertex3f     p0;
    p0.x = (-zone->viewport().bound().x / 2 + i - zone->viewport().adjust_center_x()) * cube_size_;
    p0.z = (zone->viewport().bound().y / 2 - j + zone->viewport().adjust_center_y()) * cube_size_;

    for (int z = kTerrainSurfaceLevel; z < kTerrainMaxLevels; z++) {
        com::CubeComponent *cube = zone->CubeAt(i, j, z);
        if (cube->kind() == res::Cube::CUBE_AIR) { continue; }

        res::Cube *def = DCHECK_NOTNULL(Game::This()->cube_lib()->cube(cube->kind()));

        res::Texture *tt = def->top_tex();
        res::Texture *et = def->edge_tex();

        p0.y = (-1 + (z - kTerrainSurfaceLevel)) * cube_size_;

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
        vertexs[0].vec = points[0];
        vertexs[2].tex = et->coord(1);
        vertexs[2].vec = points[2];
        vertexs[3].tex = et->coord(2);
        vertexs[3].vec = points[3];
        vertexs[1].tex = et->coord(3);
        vertexs[1].vec = points[1];

        // left
        vertexs[8].tex  = et->coord(0);
        vertexs[8].vec  = points[0];
        vertexs[9].tex  = et->coord(1);
        vertexs[9].vec  = points[4];
        vertexs[10].tex = et->coord(2);
        vertexs[10].vec = points[6];
        vertexs[11].tex = et->coord(3);
        vertexs[11].vec = points[2];

        // bottom
        vertexs[12].tex = et->coord(0);
        vertexs[12].vec = points[0];
        vertexs[14].tex = et->coord(1);
        vertexs[14].vec = points[1];
        vertexs[15].tex = et->coord(2);
        vertexs[15].vec = points[5];
        vertexs[13].tex = et->coord(3);
        vertexs[13].vec = points[4];

        // front
        vertexs[4].tex = et->coord(0);
        vertexs[4].vec = points[4];
        vertexs[5].tex = et->coord(1);
        vertexs[5].vec = points[5];
        vertexs[7].tex = et->coord(2);
        vertexs[7].vec = points[7];
        vertexs[6].tex = et->coord(3);
        vertexs[6].vec = points[6];

        // right
        vertexs[16].tex = et->coord(0);
        vertexs[16].vec = points[1];
        vertexs[18].tex = et->coord(1);
        vertexs[18].vec = points[3];
        vertexs[19].tex = et->coord(2);
        vertexs[19].vec = points[7];
        vertexs[17].tex = et->coord(3);
        vertexs[17].vec = points[5];

        // top
        vertexs[20].tex = tt->coord(0);
        vertexs[20].vec = points[2];
        vertexs[22].tex = tt->coord(1);
        vertexs[22].vec = points[6];
        vertexs[23].tex = tt->coord(2);
        vertexs[23].vec = points[7];
        vertexs[21].tex = tt->coord(3);
        vertexs[21].vec = points[3];

        glInterleavedArrays(GL_T2F_V3F, 0, vertexs);
        glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, vertex_index);
    }
}

}  // namespace sys

}  // namespace nyaa
