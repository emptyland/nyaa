#pragma once
#ifndef NYAA_GAME_TEST_SCENE_H_
#define NYAA_GAME_TEST_SCENE_H_

#include "game/vector.h"
#include "game/scene.h"

namespace nyaa {

namespace com {
class ZoneComponent;
class AvatarComponent;
}  // namespace com

namespace entity {
class PlayerEntity;
}  // namespace entity

class Game;
class EntityGridSet;

class TestScene : public Scene {
public:
    TestScene(Game *game);
    ~TestScene() final;

    const char *Name() const final { return "test-scene"; }

    void Reset() final;
    void OnKeyInput(int key, int code, int action, int mods) final;
    void Render(double delta) final;

private:
    void UpdatePlayerMovement();

    std::unique_ptr<com::ZoneComponent>   zone_;
    std::unique_ptr<entity::PlayerEntity> player_;
    std::unique_ptr<EntityGridSet> entity_grid_set_;

    float y_rolated_ = 45;
    float z_rolated_ = 45;
    float ambient_light_ = 0.8;
    Vector3f directional_light_ = {0, 0, 1};
};  // class TestScene

}  // namespace nyaa

#endif  // NYAA_GAME_TEST_SCENE_H_