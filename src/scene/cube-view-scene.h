#pragma once
#ifndef NYAA_CUBE_TILE_VIEW_SCENE_H_
#define NYAA_CUBE_TILE_VIEW_SCENE_H_

#include "scene/scene.h"

namespace nyaa {

class CubeViewScene : public Scene {
public:
    explicit CubeViewScene(Game *game);
    ~CubeViewScene() final;

    const char *Name() const final { return "cube-view-scene"; }

    void Reset() final;
    void OnKeyInput(int key, int code, int action, int mods) final;
    void Render(double delta) final;

    DISALLOW_IMPLICIT_CONSTRUCTORS(CubeViewScene);

private:
    class Core;

    std::unique_ptr<Core> core_;
};  // class CubeViewScene

}  // namespace nyaa

#endif  // NYAA_CUBE_TILE_VIEW_SCENE_H_