#pragma once
#ifndef NYAA_GAME_BOOT_SCENE_H_
#define NYAA_GAME_BOOT_SCENE_H_

#include "game/scene.h"
#include "glog/logging.h"

namespace nyaa {
namespace com {
class AvatarComponent;
}  // namespace entity

class BootScene : public Scene {
public:
    BootScene(Game *game);
    ~BootScene() final;

    const char *Name() const final { return "boot-scene"; }
    void Reset() final;
    void OnKeyInput(int key, int code, int action, int mods) final;
    void Render(double delta) final;

private:
    uint32_t vao_;
    uint32_t vbo_;
    uint32_t billboard_vbo_ = 0;
    float y_rolated_ = 45;
    float z_rolated_ = 45;
};  // class BootScene

}  // namespace nyaa

#endif  // NYAA_GAME_BOOT_SCENE_H_