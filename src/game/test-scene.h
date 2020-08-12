#pragma once
#ifndef NYAA_TEST_SCENE_H_
#define NYAA_TEST_SCENE_H_

#include "game/scene.h"

namespace nyaa {

namespace entity {
class Region;
}  // namespace entity

class Game;

class TestScene : public Scene {
public:
    TestScene(Game *game);
    ~TestScene() final;

    const char *Name() const final { return "test-scene"; }
    void Reset() final;
    void OnKeyInput(int key, int code, int action, int mods) final;
    //void OnMouseInput(double x, double y) final;
    void Render(double delta) final;

private:
    entity::Region *region_ = nullptr;
};  // class TestScene

}  // namespace nyaa

#endif  // NYAA_TEST_SCENE_H_