#pragma once
#ifndef NYAA_SCENE_TILE_VIEW_SCENE_H_
#define NYAA_SCENE_TILE_VIEW_SCENE_H_

#include "scene/scene.h"

namespace nyaa {

class TileViewScene : public Scene {
public:
    static constexpr uint32_t kTileIDStart = 200000;
    static constexpr uint32_t kTileIDStop  = 204160;
    static constexpr int      kTileSize    = (kTileIDStop - kTileIDStart) / 10;

    explicit TileViewScene(Game *game);
    ~TileViewScene() final;

    const char *Name() const final { return "tile-view-scene"; }

    void Reset() final;
    void OnKeyInput(int key, int code, int action, int mods) final;
    void Render(double delta) final;

    DISALLOW_IMPLICIT_CONSTRUCTORS(TileViewScene);

private:
    class Core;

    std::unique_ptr<Core> core_;
};  // class TileViewScene

}  // namespace nyaa

#endif  // NYAA_SCENE_TILE_VIEW_SCENE_H_