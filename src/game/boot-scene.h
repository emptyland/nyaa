#pragma once
#ifndef NYAA_GAME_BOOT_SCENE_H_
#define NYAA_GAME_BOOT_SCENE_H_

#include "game/scene.h"
#include "glog/logging.h"

namespace nyaa {
namespace entity {
class AvatarEntity;
}  // namespace entity

class BootScene : public Scene {
public:
    BootScene(Game *game);
    ~BootScene() final;

    void Reset() final;
    void OnKeyInput(int key, int code, int action, int mods) final;
    void OnMouseInput(double x, double y) final;
    void Render(double delta) final;

private:
    std::unique_ptr<entity::AvatarEntity> avatar1_;
    std::unique_ptr<entity::AvatarEntity> avatar2_;
};  // class BootScene

}  // namespace nyaa

#endif  // NYAA_GAME_BOOT_SCENE_H_