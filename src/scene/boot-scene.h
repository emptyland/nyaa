#pragma once
#ifndef NYAA_SCENE_BOOT_SCENE_H_
#define NYAA_SCENE_BOOT_SCENE_H_

#include "scene/scene.h"
#include "glog/logging.h"

namespace nyaa {
namespace com {
class AvatarComponent;
}  // namespace com

class BootScene : public Scene {
public:
    explicit BootScene(Game *game);
    ~BootScene() final;

    const char *Name() const final { return "boot-scene"; }

    void Reset() final;
    void OnKeyInput(int key, int code, int action, int mods) final;
    void Render(double delta) final;

    class UIController;
    DISALLOW_IMPLICIT_CONSTRUCTORS(BootScene);

private:
    std::unique_ptr<UIController> ui_;
};  // class BootScene

}  // namespace nyaa

#endif  // NYAA_SCENE_BOOT_SCENE_H_