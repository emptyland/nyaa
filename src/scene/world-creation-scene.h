#pragma once
#ifndef NYAA_SCENE_WORLD_CREATION_SCENE_H_
#define NYAA_SCENE_WORLD_CREATION_SCENE_H_

#include "scene/scene.h"

namespace nyaa {

class WorldCreationScene : public Scene {
public:
    explicit WorldCreationScene(Game *game);
    ~WorldCreationScene() final;

    const char *Name() const final { return "world-creation-scene"; }

    void Reset() final;
    void OnKeyInput(int key, int code, int action, int mods) final;
    void Render(double delta) final;

    class UIController;
    DISALLOW_IMPLICIT_CONSTRUCTORS(WorldCreationScene);

private:
    std::unique_ptr<UIController> ui_;
};  // class WorldCreationScene

}  // namespace nyaa

#endif  // NYAA_SCENE_WORLD_CREATION_SCENE_H_