#pragma once
#ifndef NYAA_GAME_ENTITIES_ZONE_H_
#define NYAA_GAME_ENTITIES_ZONE_H_

#include "entity/entity.h"
#include "game/constants.h"
#include "base/queue-macros.h"
#include "glog/logging.h"
#include <map>
#include <list>

namespace nyaa {

class EntitiesGrid {
public:
    EntitiesGrid() : entities_dummy_(EntityId::Of(0)) {}

    void Enter(entity::Entity *obj) {
        if (obj->grid() == this) { return; }
        QUEUE_INSERT_TAIL(dummy(), obj);
        obj->set_grid(this);
    }

    void Exit(entity::Entity *obj) {
        if (obj->grid() != this) { return; }
        QUEUE_REMOVE(obj);
        obj->set_grid(nullptr);
    }

    friend class EntitiesRegion;
private:
    entity::Entity *dummy() { return &entities_dummy_; }

    Vertex2i       local_coord_;
    entity::Entity entities_dummy_;
};  // class EntitiesGrid

class EntitiesRegion {
public:
    EntitiesRegion();
    ~EntitiesRegion();

    friend class EntitiesZone;
    DISALLOW_IMPLICIT_CONSTRUCTORS(EntitiesRegion);

private:
    Vertex2i global_coord_;
    // std::vector<entity::Entity *> entities_;
    std::map<EntityId, entity::Entity *> entities_;
    // :format
    EntitiesGrid grids_[kRegionSize][kRegionSize];
};  // class EntitiesRegion

class EntitiesZone {
public:
    EntitiesZone();
    ~EntitiesZone();

    void DidLoadRegion(int index, Vertex2i coord);
    void DidFreeRegion(int index);

    void ScrollEastToWest(Vertex2i coord);
    void ScrollWestToEast(Vertex2i coord);
    void ScrollNorthToSouth(Vertex2i coord);
    void ScrollSouthToNorth(Vertex2i coord);

    DISALLOW_IMPLICIT_CONSTRUCTORS(EntitiesZone);

private:
    EntitiesRegion *AllocateRegion(Vertex2i coord) {
        DCHECK(!free_regions_.empty());
        EntitiesRegion *region = free_regions_.front();
        free_regions_.pop_front();
        region->global_coord_ = coord;
        return region;
    }

    std::list<EntitiesRegion *> free_regions_;
    EntitiesRegion *region_ = nullptr;
    EntitiesRegion *sibling_[4];
};  // class EntitiesZone

}  // namespace nyaa

#endif  // NYAA_GAME_ENTITIES_ZONE_H_
