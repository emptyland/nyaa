#include "game/test-scene.h"
#include "game/game.h"
#include "resource/texture-library.h"
#include "component/zone-component.h"
#include "component/cube-component.h"
#include "component/avatar-component.h"
#include "system/entity-allocation-system.h"
#include "system/random-zone-system.h"
#include "system/zone-render-system.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

TestScene::TestScene(Game *game) : Scene(game) {}
TestScene::~TestScene() {}

void TestScene::Reset() {
    zone_.reset(new com::ZoneComponent());
    com::RegionComponent *region = new com::RegionComponent();
    region->set_global_coord({0, 0});
    zone_->set_region(region);
    Game::This()->random_zone()->Update(region);
    zone_->mutable_viewport()->set_center_coord({kRegionSize / 2, kRegionSize / 2});
    Game::This()->random_zone()->Update(zone_.get());

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
        case GLFW_KEY_W:
            Game::This()->zone_render()->set_rotate_angle_y(Game::This()->zone_render()->rotate_angle_y() + 2);
            break;
        case GLFW_KEY_S:
            Game::This()->zone_render()->set_rotate_angle_y(Game::This()->zone_render()->rotate_angle_y() - 2);
            break;
        case GLFW_KEY_A:
            Game::This()->zone_render()->set_rotate_angle_z(Game::This()->zone_render()->rotate_angle_z() - 2);
            break;
        case GLFW_KEY_D:
            Game::This()->zone_render()->set_rotate_angle_z(Game::This()->zone_render()->rotate_angle_z() + 2);
            break;
        default: break;
    }
}

void TestScene::Render(double delta) {

    Game::This()->zone_render()->Render(zone_.get());

    // TO 3D
    //------------------------------------------------------------------------------------------------------------------

    // Game::This()->zone_render()->

    // avatar_->AddTime(delta);
    // res::Texture *tex = avatar_->GetFrame();

    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_TEXTURE_2D);
    // glBindTexture(GL_TEXTURE_2D, tex->tex_id());
    // glColor3f(1.0, 1.0, 1.0);
    // glBegin(GL_QUADS);

    // // glTexCoord2f(tex->coord(0).x, tex->coord(0).y);
    // // glVertex3f(0, 1, 0);

    // // glTexCoord2f(tex->coord(1).x, tex->coord(1).y);
    // // glVertex3f(0.75, 1, 0);

    // // glTexCoord2f(tex->coord(2).x, tex->coord(2).y);
    // // glVertex3f(0.75, 0, 0);

    // // glTexCoord2f(tex->coord(3).x, tex->coord(3).y);
    // // glVertex3f(0, 0, 0);
    // glTexCoord2f(tex->coord(0).x, tex->coord(0).y);
    // glVertex3f(0, 2, 0);

    // glTexCoord2f(tex->coord(1).x, tex->coord(1).y);
    // glVertex3f(1.5, 2, 0);

    // glTexCoord2f(tex->coord(2).x, tex->coord(2).y);
    // glVertex3f(1.5, 0, 0);

    // glTexCoord2f(tex->coord(3).x, tex->coord(3).y);
    // glVertex3f(0, 0, 0);

    // glEnd();
    // glDisable(GL_TEXTURE_2D);
    // glDisable(GL_BLEND);

    //------------------------------------------------------------------------------------------------------------------
}

}  // namespace nyaa