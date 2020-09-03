#pragma once
#ifndef NYAA_GAME_WORLD_H_
#define NYAA_GAME_WORLD_H_

#include "base/base.h"

namespace nyaa {
namespace entity {
class PlayerEntity;
}  // namespace entity

class World final {
public:
    World();

private:
    entity::PlayerEntity *player_ = nullptr;
};  // class World

}  // namespace nyaa

#endif  // NYAA_GAME_WORLD_H_