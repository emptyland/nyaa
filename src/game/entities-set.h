#pragma once
#ifndef NYAA_GAME_ENTITIES_ZONE_H_
#define NYAA_GAME_ENTITIES_ZONE_H_

#include "entity/entity.h"
#include "game/constants.h"
#include "base/queue-macros.h"
#include "glog/logging.h"
#include <unordered_map>
#include <map>
#include <deque>
#include <queue>

namespace nyaa {
namespace entity {
class PlayerEntity;
class PlantEntity;
}  // namespace entity

class EntitiesGrid {
public:
    EntitiesGrid() = default;

    void Enter(entity::Entity *obj);

    void Exit(entity::Entity *obj) {
        if (obj->grid() != this) { return; }
        QUEUE_REMOVE(obj);
        obj->set_grid(nullptr);
    }

    friend class EntitiesSet;

private:
    entity::Entity *dummy() { return &entities_dummy_; }

    Vector2i       coord_;
    entity::Entity entities_dummy_;
};  // class EntitiesGrid

class EntitiesSet {
public:
    EntitiesSet();
    ~EntitiesSet();

    EntitiesGrid *grid(int x, int y) {
        DCHECK_GE(x, 0);
        DCHECK_LT(x, kRegionSize);
        DCHECK_GE(y, 0);
        DCHECK_LT(y, kRegionSize);
        return &grids_[x][y];
    }

    void UpdatePlayer(entity::PlayerEntity *obj);
    void UpdatePlant(entity::PlantEntity *obj);

    void Remove(entity::Entity *obj) {
        entities_.erase(obj->id());
        if (obj->grid()) {
            obj->grid()->Exit(obj);
        }
    }

    DISALLOW_IMPLICIT_CONSTRUCTORS(EntitiesSet);

private:
    void Update(const Vector3f &position, entity::Entity *obj) {
        EntitiesGrid *g = grid(position.x, position.y);
        if (!obj->grid()) {
            g->Enter(obj);
            entities_[obj->id()] = obj;
        } else {
            obj->grid()->Exit(obj);
            g->Enter(obj);
        }
    }

    std::unordered_map<EntityId, entity::Entity *, EntityHash> entities_;

    EntitiesGrid grids_[kRegionSize][kRegionSize];
};  // class EntitiesSet

}  // namespace nyaa

#endif  // NYAA_GAME_ENTITIES_ZONE_H_
