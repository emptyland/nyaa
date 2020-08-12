#include "game/test-scene.h"
#include "game/game.h"
#include "resource/texture-library.h"
#include "entity/region.h"
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
    Game::This()->entity_allocator()->Register<entity::Region>();
    region_ = Game::This()->entity_allocator()->Allocate<entity::Region>();

    sys::RandomRegionSystem random_region;
    random_region.FillRegion(region_);

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

        default: break;
    }
}

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
    glRotated(45, -1, 0, 0);

    glPushMatrix();
    glRotated(45, 0.0, 1.0, 0.0);

    glPushMatrix();
    glScaled(0.1, 0.1, 0.1);

    constexpr int      n_cubes = 12;
    constexpr Vertex2i center{kRegionSize / 2, kRegionSize / 2};
    Vertex4i           bound{center.x - n_cubes, center.y - n_cubes, n_cubes * 2, n_cubes * 2};


    //------------------------------------------------------------------------------------------------------------------

    entity::Region::Floor *surface = region_->floor(kTerrainSurfaceLevel);
    res::Cube *            cube    = game()->cube_lib()->cube(surface->cubes[0][0].kind());
    res::Texture *         tex     = cube->top_tex();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cube->top_tex()->tex_id());
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);

    int      size = 1;
    Vertex3i pos{-n_cubes, 0, n_cubes};

    for (int j = bound.y; j < bound.y + bound.h; j++) {
        for (int i = bound.x; i < bound.x + bound.w; i++) {
            cube              = game()->cube_lib()->cube(surface->cubes[i][j].kind());
            res::Texture *tex = cube->top_tex();

            // TOP:
            // -1, 1, -1
            glTexCoord2f(tex->coord(0).x, tex->coord(0).y);  // 0,0
            glVertex3f(pos.x, pos.y, pos.z);
            glTexCoord2f(tex->coord(3).x, tex->coord(3).y);  // 0,1
            glVertex3f(pos.x + size, pos.y, pos.z);
            glTexCoord2f(tex->coord(2).x, tex->coord(2).y);  // 1,1
            glVertex3f(pos.x + size, pos.y, pos.z + size);
            glTexCoord2f(tex->coord(1).x, tex->coord(1).y);  // 1,0
            glVertex3f(pos.x, pos.y, pos.z + size);

            tex = cube->edge_tex();
            glTexCoord2f(tex->coord(0).x, tex->coord(0).y);
            glVertex3f(pos.x + size, pos.y, pos.z);
            glTexCoord2f(tex->coord(3).x, tex->coord(3).y);
            glVertex3f(pos.x + size, pos.y, pos.z + size);
            glTexCoord2f(tex->coord(2).x, tex->coord(2).y);
            glVertex3f(pos.x + size, pos.y - size, pos.z + size);
            glTexCoord2f(tex->coord(1).x, tex->coord(1).y);
            glVertex3f(pos.x + size, pos.y - size, pos.z);

            glTexCoord2f(tex->coord(0).x, tex->coord(0).y);
            glVertex3f(pos.x, pos.y - size, pos.z);
            glTexCoord2f(tex->coord(3).x, tex->coord(3).y);
            glVertex3f(pos.x + size, pos.y - size, pos.z);
            glTexCoord2f(tex->coord(2).x, tex->coord(2).y);
            glVertex3f(pos.x + size, pos.y, pos.z);
            glTexCoord2f(tex->coord(1).x, tex->coord(1).y);
            glVertex3f(pos.x, pos.y, pos.z);

            pos.x += size;
        }
        pos.x = -n_cubes;
        pos.z -= size;
    }

    glEnd();

    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    //------------------------------------------------------------------------------------------------------------------
    glPushMatrix();
    glScaled(0.12, 0.12, 0.12);

    avatar_->AddTime(delta);
    tex = avatar_->GetFrame();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex->tex_id());
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);

    float move_d = 0 - avatar_->time();
    printf("move-y: %f\n", move_d);
    if (move_d < -10) {
        move_d = 10 - avatar_->time();
    }

    glTexCoord2f(tex->coord(0).x, tex->coord(0).y);
    glVertex3f(move_d - 0, 1, 0);

    glTexCoord2f(tex->coord(1).x, tex->coord(1).y);
    glVertex3f(move_d - 0.75, 1, 0);

    glTexCoord2f(tex->coord(2).x, tex->coord(2).y);
    glVertex3f(move_d - 0.75, 0, 0);

    glTexCoord2f(tex->coord(3).x, tex->coord(3).y);
    glVertex3f(move_d - 0, 0, 0);

    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    glPopMatrix();
    //------------------------------------------------------------------------------------------------------------------

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
}

}  // namespace nyaa