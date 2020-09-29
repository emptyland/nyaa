#pragma once
#ifndef NYAA_SCENE_SCENE_H_
#define NYAA_SCENE_SCENE_H_

#include "game/identifiers.h"
#include "base/base.h"

namespace nyaa {
namespace sys {
class System;
}  // namespace sys

class Game;

class Scene {
public:
    Scene(Game *game);
    virtual ~Scene();

    DEF_PTR_GETTER(Game, game);
    DEF_PTR_GETTER(sys::System, system);
    DEF_PTR_PROP_RW(Scene, prev);

    void SwitchTo(Scene *prev = nullptr);
    void DelayDispose();

    virtual const char *Name() const = 0;

    virtual void Reset();
    virtual void OnKeyInput(int key, int code, int action, int mods);
    virtual void Render(double delta);

    bool TestKeyPressed(int key);

    DISALLOW_ALL_CONSTRUCTORS(Scene);

private:
    EntityId id_;
    Game *   game_;
    sys::System *system_;
    Scene *  prev_ = nullptr;
};

}  // namespace nyaa

#endif  // NYAA_SCENE_SCENE_H_