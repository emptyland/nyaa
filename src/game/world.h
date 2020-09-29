#pragma once
#ifndef NYAA_GAME_WORLD_H_
#define NYAA_GAME_WORLD_H_

#include "game/pseudo-random-number.h"
#include "game/game.h"

namespace nyaa {
namespace com {
class ZoneComponent;
class AvatarComponent;
}  // namespace com
namespace entity {
class PlayerEntity;
}  // namespace entity

class Game;
class EntityGridSet;

class World final {
public:
    World();
    ~World();

    PseudoRandomGenerator *random() { return &random_; }

    bool Prepare();

    static inline World *This() { return Game::This()->world(); }

    DISALLOW_IMPLICIT_CONSTRUCTORS(World);

private:
    std::unique_ptr<com::ZoneComponent>   zone_;
    std::unique_ptr<entity::PlayerEntity> player_;
    std::unique_ptr<EntityGridSet>        entity_grid_set_;
    PseudoRandomGenerator                 random_;
};  // class World

}  // namespace nyaa

#endif  // NYAA_GAME_WORLD_H_