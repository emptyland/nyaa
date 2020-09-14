#pragma once
#ifndef NYAA_GAME_AVATAR_VIEW_SCENE_H_
#define NYAA_GAME_AVATAR_VIEW_SCENE_H_

#include "game/scene.h"

namespace nyaa {

class AvatarViewScene : public Scene {
public:
    explicit AvatarViewScene(Game *game);
    ~AvatarViewScene() final;

    const char *Name() const final { return "avatar-view-scene"; }

    void Reset() final;
    void OnKeyInput(int key, int code, int action, int mods) final;
    void Render(double delta) final;

    DISALLOW_IMPLICIT_CONSTRUCTORS(AvatarViewScene);

private:
    class Core;

    std::unique_ptr<Core> ctrl_;
};  // class AvatarViewScene

}  // namespace nyaa

#endif  // NYAA_GAME_AVATAR_VIEW_SCENE_H_