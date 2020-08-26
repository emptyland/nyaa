#include "game/test-scene.h"
#include "game/game.h"
#include "game/matrix.h"
#include "resource/texture-library.h"
#include "resource/font-library.h"
#include "resource/shader-library.h"
#include "entity/player-entity.h"
#include "component/zone-component.h"
#include "component/cube-component.h"
#include "component/avatar-component.h"
#include "system/entity-allocation-system.h"
#include "system/geometry-transform-system.h"
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

    game()->zone_render()->Prepare();
    // TODO: player_->mutable_movement()->set_coord(zone_->viewport().center_coord());
}

static constexpr float speed = 10;

void TestScene::OnKeyInput(int key, int code, int action, int mods) {

    switch (key) {
        case GLFW_KEY_ESCAPE:
        case GLFW_KEY_R:
            DelayDispose();
            DCHECK_NOTNULL(prev())->SwitchTo(nullptr);
            break;
        case GLFW_KEY_SPACE: {
            if (player_->movement().speed().z == 0) { player_->mutable_movement()->mutable_speed()->z = 10; }
            // UpdatePlayerMovement();
        } break;
        default: break;
    }
}

void TestScene::Render(double delta) {
    int action = 0, command = 0;
    if (action = glfwGetKey(game()->window(), GLFW_KEY_W); action == GLFW_PRESS) {
        player_->mutable_movement()->mutable_speed()->y = -speed;
        command++;
    } else {
        player_->mutable_movement()->mutable_speed()->y = 0;
    }
    if (action = glfwGetKey(game()->window(), GLFW_KEY_S); action == GLFW_PRESS) {
        player_->mutable_movement()->mutable_speed()->y = speed;
        command++;
    } else {
        if (player_->mutable_movement()->mutable_speed()->y > 0) {
            player_->mutable_movement()->mutable_speed()->y = 0;
        }
    }
    if (action = glfwGetKey(game()->window(), GLFW_KEY_A); action == GLFW_PRESS) {
        player_->mutable_movement()->mutable_speed()->x = -speed;
        command++;
    } else {
        player_->mutable_movement()->mutable_speed()->x = 0;
    }
    if (action = glfwGetKey(game()->window(), GLFW_KEY_D); action == GLFW_PRESS) {
        player_->mutable_movement()->mutable_speed()->x = speed;
        command++;
    } else {
        if (player_->mutable_movement()->mutable_speed()->x > 0) {
            player_->mutable_movement()->mutable_speed()->x = 0;
        }
    }

    if (command == 0) {
        player_->mutable_movement()->mutable_speed()->x = 0;
        player_->mutable_movement()->mutable_speed()->y = 0;
    }

    if (glfwGetKey(game()->window(), GLFW_KEY_UP) == GLFW_PRESS) {
        y_rolated_ -= 2;
    } else if (glfwGetKey(game()->window(), GLFW_KEY_DOWN) == GLFW_PRESS) {
        y_rolated_ += 2;
    } else if (glfwGetKey(game()->window(), GLFW_KEY_LEFT) == GLFW_PRESS) {
        z_rolated_ -= 2;
    } else if (glfwGetKey(game()->window(), GLFW_KEY_RIGHT) == GLFW_PRESS) {
        z_rolated_ += 2;
    } else if (glfwGetKey(game()->window(), GLFW_KEY_H) == GLFW_PRESS) {
        if (ambient_light_ < 1) {
            ambient_light_ += 0.01;
        }
    } else if (glfwGetKey(game()->window(), GLFW_KEY_J) == GLFW_PRESS) {
        if (ambient_light_ > 0) {
            ambient_light_ -= 0.01;
        }
    }


    game()->actor_movement()->Update(player_->mutable_movement(), zone_.get(), 0.3,
                                     game()->frame_delta_time() /*delta*/, false);

    if (player_->movement().is_moving()) {
        zone_->mutable_viewport()->set_center_coord({player_->movement().coord().x, player_->movement().coord().y});
        game()->zone_loader()->Update(zone_.get());
    }

    res::ShaderProgramScope shader(game()->shader_lib()->block_program());
    shader->SetDiffuseMaterial({0.6, 0.6, 0.6});
    shader->SetDiffuseLight({0.7, 0.7, 0.7});
    shader->SetAmbientMaterial({0.8, 0.8, 0.8});
    shader->SetAmbientLight({ambient_light_, ambient_light_, ambient_light_});
    shader->SetSpecularMaterial({0.7, 0.7, 0.7});
    shader->SetSpecularLight({1.0, 1.0, 1.0});

    Matrix mat;
    mat.Identity();

    Matrix view_mat;
    view_mat.Translate(0, 0, -2);
    mat.Rotate(1, 0, 0, y_rolated_);
    view_mat.Multiply(mat);
    mat.Rotate(0, 0, 1, z_rolated_);
    view_mat.Multiply(mat);
    mat.Scale(0.1, 0.1, 0.1);
    view_mat.Multiply(mat);

    Vector4f camera{0, 0, -2, 1};

    Matrix model_mat;
    model_mat.Identity();

    Matrix proj_mat;
    proj_mat.Perspective(45, static_cast<float>(game()->fb_w()) / game()->fb_h(), 0.1, 100);

    shader->SetViewMatrix(view_mat);
    shader->SetProjectionMatrix(proj_mat);
    shader->SetModelMatrix(model_mat);
    shader->SetDirectionalLight({.0, .0, 1.0});
    shader->SetCameraPosition({camera.x, camera.y, camera.z});

    // const vec3 directionalLight = vec3(0, 1.0, 1.0);
    // const vec3 cameraPosition   = vec3(0, 0, 2);

    game()->zone_render()->RenderTerrain(zone_.get());

    // {
    //     char buf[128];
    //     ::snprintf(buf, arraysize(buf), "x=%0.2f, y=%0.2f, z=%0.2f", zone_->viewport().center_coord().x,
    //                zone_->viewport().center_coord().y, player_->movement().coord().z);
    //     game()->font_lib()->default_face()->Render(buf, 0, 32, {1, 1, 0});
    // }
}

}  // namespace nyaa