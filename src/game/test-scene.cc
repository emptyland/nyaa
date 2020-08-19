#include "game/test-scene.h"
#include "game/game.h"
#include "resource/texture-library.h"
#include "resource/font-library.h"
#include "entity/player-entity.h"
#include "component/zone-component.h"
#include "component/cube-component.h"
#include "component/avatar-component.h"
#include "system/entity-allocation-system.h"
#include "system/random-zone-system.h"
#include "system/zone-render-system.h"
#include "system/zone-loading-system.h"
#include "system/actor-movement-system.h"
#include "system/avatar-render-system.h"
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
    game()->random_zone()->Update(region);
    zone_->mutable_viewport()->set_center_coord({kRegionSize / 2, kRegionSize / 2});
    game()->random_zone()->Update(zone_.get());

    game()->entity_allocator()->Register<entity::PlayerEntity>();
    player_.reset(
        game()->entity_allocator()->New<entity::PlayerEntity>(game()->NextEntityId(), ResourceId::Of(100000)));
    player_->mutable_movement()->mutable_coord()->z = kTerrainSurfaceLevel + 2;
    player_->mutable_movement()->mutable_coord()->x = zone_->viewport().center_coord().x;
    player_->mutable_movement()->mutable_coord()->y = zone_->viewport().center_coord().y;

    // TODO: player_->mutable_movement()->set_coord(zone_->viewport().center_coord());
}

void TestScene::OnKeyInput(int key, int code, int action, int mods) {
    static constexpr float speed = 30;

    switch (key) {
        case GLFW_KEY_ESCAPE:
        case GLFW_KEY_R:
            DelayDispose();
            DCHECK_NOTNULL(prev())->SwitchTo(nullptr);
            break;
        case GLFW_KEY_W: {
            player_->mutable_movement()->set_speed({0, -speed, 0});
            UpdatePlayerMovement();
        } break;
        case GLFW_KEY_S: {
            player_->mutable_movement()->set_speed({0, speed, 0});
            UpdatePlayerMovement();
        } break;
        case GLFW_KEY_A: {
            player_->mutable_movement()->set_speed({-speed, 0, 0});
            UpdatePlayerMovement();
        } break;
        case GLFW_KEY_D: {
            player_->mutable_movement()->set_speed({speed, 0, 0});
            UpdatePlayerMovement();
        } break;
        case GLFW_KEY_SPACE: {
            player_->mutable_movement()->set_speed({0.01, 70, 0});
            UpdatePlayerMovement();
        } break;
        case GLFW_KEY_UP:
            // :format
            game()->zone_render()->set_rotate_angle_y(game()->zone_render()->rotate_angle_y() + 2);
            break;
        case GLFW_KEY_DOWN:
            game()->zone_render()->set_rotate_angle_y(game()->zone_render()->rotate_angle_y() - 2);
            break;
        case GLFW_KEY_LEFT:
            game()->zone_render()->set_rotate_angle_z(game()->zone_render()->rotate_angle_z() - 2);
            break;
        case GLFW_KEY_RIGHT:
            game()->zone_render()->set_rotate_angle_z(game()->zone_render()->rotate_angle_z() + 2);
            break;
        default: break;
    }
}

void TestScene::UpdatePlayerMovement() {
    game()->actor_movement()->Update(player_->mutable_movement(), zone_.get(), 30,
                                     game()->frame_delta_time() /*delta*/);
    zone_->mutable_viewport()->set_center_coord({player_->movement().coord().x, player_->movement().coord().y});
    game()->zone_loader()->Update(zone_.get());
}

void TestScene::Render(double delta) {

    game()->zone_render()->Render(zone_.get());
    game()->avatar_render()->Render(player_->mutable_movement(), player_->mutable_avatar(), delta);

    {
        char buf[128];
        ::snprintf(buf, arraysize(buf), "x=%0.2f, y=%0.2f", zone_->viewport().center_coord().x,
                   zone_->viewport().center_coord().y);
        game()->font_lib()->default_face()->Render(buf, 0, 32, {1, 1, 0});
    }
}

}  // namespace nyaa