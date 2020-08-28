#include "game/test-scene.h"
#include "game/game.h"
#include "game/matrix.h"
#include "game/entities-set.h"
#include "resource/texture-library.h"
#include "resource/font-library.h"
#include "resource/shader-library.h"
#include "entity/player-entity.h"
#include "entity/plant-entity.h"
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
    entities_set_.reset(new EntitiesSet);

    zone_.reset(new com::ZoneComponent());
    com::RegionComponent *region = new com::RegionComponent();
    region->set_global_coord({0, 0});
    zone_->set_region(1, 1, region);
    game()->random_zone()->Update(region);
    for (int i = 0; i < region->plants_size(); i++) {
        // entities_set_->UpdatePlant()
        entity::PlantEntity *plant = game()->entity_allocator()->New<entity::PlantEntity>(region->plant(i));
        entities_set_->UpdatePlant(plant);
    }

    zone_->mutable_viewport()->set_center_coord({kRegionSize / 2, kRegionSize / 2});

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
    int command = 0;
    if (glfwGetKey(game()->window(), GLFW_KEY_W) == GLFW_PRESS) {
        player_->mutable_movement()->mutable_speed()->y = speed;
        // zone_->mutable_viewport()->mutable_center_coord()->y += 0.5;
        command++;
    }
    if (glfwGetKey(game()->window(), GLFW_KEY_S) == GLFW_PRESS) {
        player_->mutable_movement()->mutable_speed()->y = -speed;
        command++;
        // if (zone_->mutable_viewport()->mutable_center_coord()->y > 0) {
        //     zone_->mutable_viewport()->mutable_center_coord()->y -= 0.5;
        //     command++;
        // }
    }
    if (glfwGetKey(game()->window(), GLFW_KEY_A) == GLFW_PRESS) {
        player_->mutable_movement()->mutable_speed()->x = -speed;
        command++;
        // if (zone_->mutable_viewport()->mutable_center_coord()->x > 0) {
        //     zone_->mutable_viewport()->mutable_center_coord()->x -= 0.5;
        //     command++;
        // }
    }
    if (glfwGetKey(game()->window(), GLFW_KEY_D) == GLFW_PRESS) {
        // zone_->mutable_viewport()->mutable_center_coord()->x += 0.5;
        player_->mutable_movement()->mutable_speed()->x = speed;
        command++;
    }
    if (glfwGetKey(game()->window(), GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (player_->movement().speed().z == 0) { player_->mutable_movement()->mutable_speed()->z = 10; }
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
        // if (ambient_light_ < 1) { ambient_light_ += 0.01; }
        directional_light_.y -= 0.1;
    } else if (glfwGetKey(game()->window(), GLFW_KEY_J) == GLFW_PRESS) {
        // if (ambient_light_ > 0) { ambient_light_ -= 0.01; }
        directional_light_.y += 0.1;
    } else if (glfwGetKey(game()->window(), GLFW_KEY_K) == GLFW_PRESS) {
        // if (ambient_light_ > 0) { ambient_light_ -= 0.01; }
        directional_light_.z -= 0.1;
    } else if (glfwGetKey(game()->window(), GLFW_KEY_L) == GLFW_PRESS) {
        // if (ambient_light_ > 0) { ambient_light_ -= 0.01; }
        directional_light_.z += 0.1;
    }

    if (zone_->center()) {
        game()->actor_movement()->Update(player_->mutable_movement(), zone_.get(), 0.3,
                                         game()->frame_delta_time() /*delta*/, false);
    }

    if (!zone_->center() || command > 0) {
        zone_->mutable_viewport()->set_center_coord({player_->movement().coord().x, player_->movement().coord().y});
        game()->zone_loader()->Update(zone_.get());
    }
    entities_set_->UpdatePlayer(player_.get());

    res::BlockShaderProgram *shader = game()->shader_lib()->block_program();
    shader->Use();
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
    shader->SetDirectionalLight(directional_light_);
    shader->SetCameraPosition({camera.x, camera.y, camera.z});

    // const vec3 directionalLight = vec3(0, 1.0, 1.0);
    // const vec3 cameraPosition   = vec3(0, 0, 2);

    game()->zone_render()->RenderTerrain(zone_.get());

    for (int y = 0; y < zone_->viewport().bound().y; y++) {
        for (int x = 0; x < zone_->viewport().bound().x; x++) {
            for (entity::Entity *obj : *entities_set_->ViewGrid(zone_->viewport(), x, y)) {
                if (obj->Is<entity::PlayerEntity>()) {
                    game()->avatar_render()->Render(player_->mutable_movement(), player_->mutable_avatar(), delta);
                }
                // TODO:
            }
        }
    }


    shader->Unuse();
    {
        char buf[128];
        ::snprintf(buf, arraysize(buf), "x=%0.2f, y=%0.2f, z=%0.2f", zone_->viewport().center_coord().x,
                   zone_->viewport().center_coord().y, player_->movement().coord().z);
        game()->font_lib()->default_face()->Render(buf, 0, 32, {1, 1, 0});
    }
}

}  // namespace nyaa