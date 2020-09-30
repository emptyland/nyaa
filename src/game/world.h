#pragma once
#ifndef NYAA_GAME_WORLD_H_
#define NYAA_GAME_WORLD_H_

#include "game/pseudo-random-number.h"
#include "game/game.h"

namespace nyaa {
namespace base {
class Arena;
}  // namespace base
namespace com {
class ZoneComponent;
class AvatarComponent;
}  // namespace com
namespace entity {
class PlayerEntity;
}  // namespace entity

class Game;
class EntityGridSet;
class WorldMap;

class World final {
public:
    World();
    ~World();

    DEF_PTR_GETTER(WorldMap, map);
    DEF_PTR_GETTER(com::ZoneComponent, zone);
    DEF_PTR_GETTER(entity::PlayerEntity, player);
    PseudoRandomGenerator *random() { return &random_; }

    bool Prepare(base::Arena *arena, std::string_view world_id);

    static inline World *This() { return Game::This()->world(); }

    DISALLOW_IMPLICIT_CONSTRUCTORS(World);

private:
    WorldMap *            map_             = nullptr;
    com::ZoneComponent *  zone_            = nullptr;
    entity::PlayerEntity *player_          = nullptr;
    EntityGridSet *       entity_grid_set_ = nullptr;
    PseudoRandomGenerator random_;
};  // class World

}  // namespace nyaa

#endif  // NYAA_GAME_WORLD_H_