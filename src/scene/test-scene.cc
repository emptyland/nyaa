#include "scene/test-scene.h"
#include "resource/texture-library.h"
#include "resource/font-library.h"
#include "resource/shader-library.h"
#include "entity/player-entity.h"
#include "entity/plant-entity.h"
#include "entity/actor-entity.h"
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
#include "system/sprite-render-system.h"
#include "system/actor-billboard-render-system.h"
#include "system/actor-ai-system.h"
#include "system/impact-checking-system.h"
#include "game/game.h"
#include "game/matrix.h"
#include "game/entity-grid-set.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

TestScene::TestScene(Game *game) : Scene(game) {}
TestScene::~TestScene() {}

void TestScene::Reset() {
    entity_grid_set_.reset(new EntityGridSet);

    zone_.reset(new com::ZoneComponent());
    com::RegionComponent *region = new com::RegionComponent();
    region->set_global_coord({0, 0});
    zone_->set_region(1, 1, region);
    game()->random_zone()->Update(region);
    for (int i = 0; i < region->plants_size(); i++) {
        // entity_grid_set_->UpdatePlant()
        entity::PlantEntity *plant = game()->entity_allocator()->New<entity::PlantEntity>(region->plant(i));
        entity_grid_set_->UpdatePlant(plant);
    }

    zone_->mutable_viewport()->set_center_coord({kRegionSize / 2, kRegionSize / 2});

    game()->entity_allocator()->Register<entity::PlayerEntity>();
    player_.reset(
        game()->entity_allocator()->New<entity::PlayerEntity>(game()->NextEntityId(), ResourceId::Of(101110)));
    player_->mutable_movement()->mutable_coord()->z = kTerrainSurfaceLevel + 2;
    player_->mutable_movement()->mutable_coord()->x = zone_->viewport().center_coord().x;
    player_->mutable_movement()->mutable_coord()->y = zone_->viewport().center_coord().y;

    entity::ActorEntity *actor =
        game()->entity_allocator()->New<entity::ActorEntity>(game()->NextEntityId(), ResourceId::Of(100010));
    *actor->mutable_movement() = player_->movement();
    actor->mutable_movement()->mutable_coord()->x += 1;
    actor->mutable_movement()->mutable_coord()->y += 1;
    actor->mutable_movement()->mutable_coord()->z = kTerrainSurfaceLevel + 2;

    entity_grid_set_->UpdateActor(actor);

    game()->zone_render()->Reset();
    // TODO: player_->mutable_movement()->set_coord(zone_->viewport().center_coord());
}

static constexpr float speed = 5;

void TestScene::OnKeyInput(int key, int code, int action, int mods) {}

int TestScene::HandleKeyInput() {
    if (glfwGetKey(game()->window(), GLFW_KEY_R) == GLFW_PRESS ||
        glfwGetKey(game()->window(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        DelayDispose();
        DCHECK_NOTNULL(prev())->SwitchTo(nullptr);
        return 0;
    }

    int command = 0;
    if (glfwGetKey(game()->window(), GLFW_KEY_W) == GLFW_PRESS) {
        player_->mutable_movement()->mutable_speed()->y = speed;
        // zone_->mutable_viewport()->mutable_center_coord()->y += 0.5;
        command++;
    }
    if (glfwGetKey(game()->window(), GLFW_KEY_S) == GLFW_PRESS) {
        player_->mutable_movement()->mutable_speed()->y = -speed;
        command++;
    }
    if (glfwGetKey(game()->window(), GLFW_KEY_A) == GLFW_PRESS) {
        player_->mutable_movement()->mutable_speed()->x = -speed;
        command++;
    }
    if (glfwGetKey(game()->window(), GLFW_KEY_D) == GLFW_PRESS) {
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
        directional_light_.y -= 0.1;
    } else if (glfwGetKey(game()->window(), GLFW_KEY_J) == GLFW_PRESS) {
        directional_light_.y += 0.1;
    } else if (glfwGetKey(game()->window(), GLFW_KEY_K) == GLFW_PRESS) {
        directional_light_.z -= 0.1;
    } else if (glfwGetKey(game()->window(), GLFW_KEY_L) == GLFW_PRESS) {
        directional_light_.z += 0.1;
    } else if (glfwGetKey(game()->window(), GLFW_KEY_F) == GLFW_PRESS) {
        directional_light_.x -= 0.1;
    } else if (glfwGetKey(game()->window(), GLFW_KEY_G) == GLFW_PRESS) {
        directional_light_.x += 0.1;
    }

    if (glfwGetKey(game()->window(), GLFW_KEY_O) == GLFW_PRESS) {
        ambient_light_ = 1.0;
    } else if (glfwGetKey(game()->window(), GLFW_KEY_P) == GLFW_PRESS) {
        ambient_light_ = 0.1;
    }
    return command;
}

void TestScene::Render(double delta) {
    int command = !game()->break_input() ? HandleKeyInput() : 0;

    sys::ImpactCheckingSystem impact(zone_.get(), entity_grid_set_.get());

    // glViewport()

    if (zone_->center()) {
        game()->actor_movement()->Update(player_->id(), player_->mutable_movement(), 0.3, &impact, delta);
    }

    if (!zone_->center() || command > 0) {
        zone_->mutable_viewport()->set_center_coord({player_->movement().coord().x, player_->movement().coord().y});
        game()->zone_loader()->Update(zone_.get());
    }
    entity_grid_set_->UpdatePlayer(player_.get());

    res::BlockShaderProgram *    bk_shader = game()->shader_lib()->block_program();
    res::BillboardShaderProgram *bb_shader = game()->shader_lib()->billboard_program();
    bk_shader->Use();
    bk_shader->SetDiffuseMaterial({0.6, 0.6, 0.6});
    bk_shader->SetAmbientMaterial({0.8, 0.8, 0.8});
    bk_shader->SetSpecularMaterial({0.7, 0.7, 0.7});

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

    bk_shader->SetViewMatrix(view_mat);
    bk_shader->SetProjectionMatrix(proj_mat);
    bk_shader->SetModelMatrix(model_mat);
    bk_shader->SetDirectionalLight(directional_light_);
    bk_shader->SetDirectionalLightColor(Vec3(ambient_light_, ambient_light_, ambient_light_));
    bk_shader->SetPointLightColor(Vec3(0, 0, 0));
    bk_shader->SetPointLightConstant(1);
    bk_shader->SetPointLightLinear(0.09);
    bk_shader->SetPointLightQuadratic(0.032);
    bk_shader->SetPointLightPosition(Vec3(0, 0, 2));
    bk_shader->SetCameraPosition({camera.x, camera.y, camera.z});

    game()->zone_render()->RenderTerrain(zone_.get());

    Vector3f view = Vec3(zone_->viewport().center_coord(), kTerrainSurfaceLevel);

    entity::ActorEntity *actor = nullptr;
    // for (int y = 0; y < zone_->viewport().bound().y; y++) {
    for (int y = zone_->viewport().bound().y - 1; y >= 0; y--) {
        for (int x = 0; x < zone_->viewport().bound().x; x++) {
            // for (int x = zone_->viewport().bound().x - 1; x >= 0; x--) {
            for (entity::Entity *obj : *entity_grid_set_->ViewGrid(zone_->viewport(), x, y)) {
                if (obj->Is<entity::PlayerEntity>()) {
                    game()->avatar_render()->Render(player_->mutable_movement(), player_->mutable_avatar(), nullptr,
                                                    delta);
                } else if (obj->Is<entity::PlantEntity>()) {
                    game()->sprite_render()->RenderPlant(view, obj->AsOrNull<entity::PlantEntity>()->plant(), delta);
                } else if (obj->Is<entity::ActorEntity>()) {
                    actor = obj->AsOrNull<entity::ActorEntity>();

                    game()->actor_ai()->Update(actor->mutable_ai_state(), actor->mutable_movement(),
                                               actor->mutable_nature(), zone_.get(), nullptr, delta);
                    game()->actor_movement()->Update(actor->id(), actor->mutable_movement(), 0.3, &impact, delta);
                    entity_grid_set_->UpdateActor(actor);
                    game()->avatar_render()->Render(actor->mutable_movement(), actor->mutable_avatar(), &view, delta);
                }
            }
        }
    }

    Vector4f coord = {0, 0, 0, 0};
    if (actor) {
        bb_shader->Use();
        bb_shader->SetViewMatrix(view_mat);
        bb_shader->SetProjectionMatrix(proj_mat);
        game()->actor_billboard()->Render(actor->movement().coord(), Vec3(1.0, 1.0, 1.0), actor->id(),
                                          actor->mutable_nature(), &view);

        // Matrix<float> model_view;
        Vector3f d = actor->movement().coord() - view;
        model_mat.Scale(0.5, 0.5, 0.5);
        mat.Translate(d.x, d.y, d.z);
        model_mat.Multiply(mat);

        Matrix<float>::Multiply(model_mat, Vec4(d, 1), &coord);
        Matrix<float>::Multiply(view_mat, coord, &coord);
        Matrix<float>::Multiply(proj_mat, coord, &coord);

        coord.x /= coord.w;
        coord.y /= coord.w;
        coord.z /= coord.w;

        // coord.x = -coord.x;
        // coord.y = -coord.y;
        coord.x = game()->fb_w() / 2 + coord.x + game()->fb_w() / 2 * coord.x;
        coord.y = game()->fb_h() / 2 + coord.y + game()->fb_h() / 2 * coord.y;

        // DLOG(INFO) << "coord = " << coord.x << ", " << coord.y << ", " << coord.z;
    }

    bk_shader->Unuse();
    {
        char buf[128];
        ::snprintf(buf, arraysize(buf), "x=%0.2f, y=%0.2f, z=%0.2f, light=%0.0f,%0.0f,%0.0f",
                   zone_->viewport().center_coord().x, zone_->viewport().center_coord().y,
                   player_->movement().coord().z, coord.x, coord.y, coord.z);
        game()->font_lib()->default_face()->Render(buf, 0, 32, {1, 0, 0});

        game()->font_lib()->default_face()->Render("X", coord.x, coord.y, {0, 1, 1});
    }
}

}  // namespace nyaa