#pragma once
#ifndef NYAA_GAME_SCENE_H_
#define NYAA_GAME_SCENE_H_

#include "game/identifiers.h"
#include "base/base.h"

namespace nyaa {

class Game;

class Scene {
public:
    Scene(Game *game);
    virtual ~Scene();

    DEF_PTR_GETTER(Game, game);

    virtual void Reset();
    virtual void OnKeyInput(int key, int code, int action, int mods);
    virtual void OnMouseInput(double x, double y);
    virtual void Render(double delta);

    DISALLOW_ALL_CONSTRUCTORS(Scene);
private:
    Game *game_;
    EntityId id_;
};
    
} // namespace nyaa


#endif // NYAA_GAME_SCENE_H_