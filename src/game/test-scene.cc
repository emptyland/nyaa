#include "game/test-scene.h"
#include "game/game.h"
#include "resource/texture-library.h"
#include "component/zone-component.h"
#include "component/cube-component.h"
#include "component/avatar-component.h"
#include "system/entity-allocation-system.h"
#include "system/random-region-system.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

TestScene::TestScene(Game *game) : Scene(game) {}
TestScene::~TestScene() {}

void TestScene::Reset() {
    region_.reset(new com::RegionComponent());

    sys::RandomRegionSystem random_region;
    random_region.Update(region_.get());

    res::Avatar *def = game()->avatar_lib()->FindOrNull(ResourceId::Of(100000));
    DCHECK(def != nullptr);
    avatar_.reset(new com::AvatarComponent(def));
    avatar_->set_dir(res::Avatar::kRight);
    avatar_->set_speed(0.9);
}

void TestScene::OnKeyInput(int key, int code, int action, int mods) {
    switch (key) {
        case GLFW_KEY_ESCAPE:
        case GLFW_KEY_R:
            DelayDispose();
            DCHECK_NOTNULL(prev())->SwitchTo(nullptr);
            break;
        case GLFW_KEY_W: angle_y_ += 1; break;
        case GLFW_KEY_S: angle_y_ -= 1; break;
        case GLFW_KEY_A: angle_z_ -= 1; break;
        case GLFW_KEY_D: angle_z_ += 1; break;
        default: break;
    }
}

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

// static const GLuint vertex_index[][4] = {
//     0, 2, 3, 1, 0, 4, 6, 2, 0, 1, 5, 4, 4, 5, 7, 6, 1, 3, 7, 5, 2, 6, 7, 3,
// };

void TestScene::Render(double delta) {
    // TO 3D
    glViewport(0, 0, game()->fb_w(), game()->fb_h());

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glFrustum(0, game()->fb_w(), 0, game()->fb_h(), -100.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glRotated(angle_y_, -1, 0, 0);

    glPushMatrix();
    glRotated(angle_z_, 0.0, 1.0, 0.0);

    glPushMatrix();
    glScaled(0.14, 0.14, 0.14);

    constexpr int      n_cubes = 20;
    constexpr Vertex2i center{kRegionSize / 2, kRegionSize / 2};
    Vertex4i           bound{center.x - n_cubes, center.y - n_cubes, n_cubes * 2, n_cubes * 2};

    //------------------------------------------------------------------------------------------------------------------
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    com::RegionComponent::Floor *surface = region_->floor(kTerrainSurfaceLevel);
    res::Cube *                  cube    = game()->cube_lib()->cube(surface->cubes[0][0].kind());
    res::Texture *               tex     = cube->top_tex();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cube->top_tex()->tex_id());
    glColor3f(1.0, 1.0, 1.0);
    // glBegin(GL_QUADS);

    int      size = 1;
    Vertex3i pos{-n_cubes, -1, n_cubes};

    VertexBundle vertexs[24];  // Texture + vertex
    for (int j = bound.y; j < bound.y + bound.h; j++) {
        for (int i = bound.x; i < bound.x + bound.w; i++) {
            cube             = game()->cube_lib()->cube(surface->cubes[i][j].kind());
            res::Texture *tt = cube->top_tex();
            res::Texture *et = cube->edge_tex();

            Vertex3f p0;
            p0.x = pos.x;
            p0.y = pos.y;
            p0.z = pos.z;

            Vertex3f points[8] = {p0, p0, p0, p0, p0, p0, p0, p0};
            points[1].x += size;
            points[2].y += size;
            points[3].x += size;
            points[3].y += size;
            points[4].z += size;
            points[5].x += size;
            points[5].z += size;
            points[6].y += size;
            points[6].z += size;
            points[7].x += size;
            points[7].y += size;
            points[7].z += size;

            // back
            vertexs[0].tex = tt->coord(0);
            vertexs[0].vec = points[0];
            vertexs[2].tex = tt->coord(1);
            vertexs[2].vec = points[2];
            vertexs[3].tex = tt->coord(2);
            vertexs[3].vec = points[3];
            vertexs[1].tex = tt->coord(3);
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
            // break;
            pos.x += size;
        }
        pos.x = -n_cubes;
        pos.z -= size;
    }

    //------------------------------------------------------------------------------------------------------------------

    avatar_->AddTime(delta);
    tex = avatar_->GetFrame();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex->tex_id());
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);

    // glTexCoord2f(tex->coord(0).x, tex->coord(0).y);
    // glVertex3f(0, 1, 0);

    // glTexCoord2f(tex->coord(1).x, tex->coord(1).y);
    // glVertex3f(0.75, 1, 0);

    // glTexCoord2f(tex->coord(2).x, tex->coord(2).y);
    // glVertex3f(0.75, 0, 0);

    // glTexCoord2f(tex->coord(3).x, tex->coord(3).y);
    // glVertex3f(0, 0, 0);
    glTexCoord2f(tex->coord(0).x, tex->coord(0).y);
    glVertex3f(0, 2, 0);

    glTexCoord2f(tex->coord(1).x, tex->coord(1).y);
    glVertex3f(1.5, 2, 0);

    glTexCoord2f(tex->coord(2).x, tex->coord(2).y);
    glVertex3f(1.5, 0, 0);

    glTexCoord2f(tex->coord(3).x, tex->coord(3).y);
    glVertex3f(0, 0, 0);

    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    //------------------------------------------------------------------------------------------------------------------

    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
}

}  // namespace nyaa