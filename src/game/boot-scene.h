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
    explicit BootScene(Game *game);
    ~BootScene() final;

    const char *Name() const final { return "boot-scene"; }
    void Reset() final;
    void OnKeyInput(int key, int code, int action, int mods) final;
    void Render(double delta) final;

    DISALLOW_IMPLICIT_CONSTRUCTORS(BootScene);
private:
    class UIController;

    std::unique_ptr<UIController> ui_;
};  // class BootScene

}  // namespace nyaa

#endif  // NYAA_GAME_BOOT_SCENE_H_