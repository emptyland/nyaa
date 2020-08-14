#pragma once
#ifndef NYAA_TEST_SCENE_H_
#define NYAA_TEST_SCENE_H_

#include "game/scene.h"

namespace nyaa {

namespace com {
class RegionComponent;
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
    std::unique_ptr<com::RegionComponent> region_;
    std::unique_ptr<com::AvatarComponent> avatar_;
    float                                 angle_y_ = 45;
    float                                 angle_z_ = 45;
};  // class TestScene

}  // namespace nyaa

#endif  // NYAA_TEST_SCENE_H_