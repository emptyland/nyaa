#pragma once
#ifndef NYAA_TEST_SCENE_H_
#define NYAA_TEST_SCENE_H_

#include "game/scene.h"

namespace nyaa {

namespace entity {
class Region;
}  // namespace entity

namespace com {
class AvatarComponent;
}  // namespace com

class Game;

class TestScene : public Scene {
public:
    TestScene(Game *game);
    ~TestScene() final;

    const char *Name() const final { return "test-scene"; }

    void Reset() final;

    void OnKeyInput(int key, int code, int action, int mods) final;

    void Render(double delta) final;

private:
    entity::Region *                      region_ = nullptr;
    std::unique_ptr<com::AvatarComponent> avatar_;
    float angle_y_ = 45;
    float angle_z_ = 45;
};  // class TestScene

}  // namespace nyaa

#endif  // NYAA_TEST_SCENE_H_