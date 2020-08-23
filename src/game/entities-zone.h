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

    Vector2i       local_coord_;
    entity::Entity entities_dummy_;
};  // class EntitiesGrid

class EntitiesRegion {
public:
    EntitiesRegion();
    ~EntitiesRegion();

    friend class EntitiesZone;
    DISALLOW_IMPLICIT_CONSTRUCTORS(EntitiesRegion);

private:
    Vector2i global_coord_;
    // std::vector<entity::Entity *> entities_;
    std::map<EntityId, entity::Entity *> entities_;
    // :format
    EntitiesGrid grids_[kRegionSize][kRegionSize];
};  // class EntitiesRegion

class EntitiesZone {
public:
    EntitiesZone();
    ~EntitiesZone();

    void DidLoadRegion(int index, Vector2i coord);
    void DidFreeRegion(int index);

    void ScrollEastToWest(Vector2i coord);

    void ScrollEast(Vector2i coord) {
        ReAllocateIfNeeded(0, {coord.x + kRegionSize, coord.y});
        FreeIfNeeded(1);
        FreeIfNeeded(2);
    }

    void ScrollWestToEast(Vector2i coord);

    void ScrollWest(Vector2i coord) {
        ReAllocateIfNeeded(0, {coord.x - kRegionSize, coord.y});
        FreeIfNeeded(1);
        FreeIfNeeded(2);
    }

    void ScrollNorthToSouth(Vector2i coord);

    void ScrollNorth(Vector2i coord) {
        ReAllocateIfNeeded(0, {coord.x, coord.y - kRegionSize});
        FreeIfNeeded(1);
        FreeIfNeeded(2);
    }

    void ScrollSouthToNorth(Vector2i coord);

    void ScrollSouth(Vector2i coord) {
        ReAllocateIfNeeded(0, {coord.x, coord.y + kRegionSize});
        FreeIfNeeded(1);
        FreeIfNeeded(2);
    }

    void ScrollSEToSW(Vector2i coord);
    void ScrollSEToNE(Vector2i coord);
    void ScrollSEToNW(Vector2i coord);

    //  SE | 0
    //  ---+---
    //   2 | 1
    void ScrollSE(Vector2i coord) {
        ReAllocateIfNeeded(0, {coord.x + kRegionSize, coord.y});
        ReAllocateIfNeeded(1, {coord.x + kRegionSize, coord.y + kRegionSize});
        ReAllocateIfNeeded(2, {coord.x, coord.y + kRegionSize});
    }

    void ScrollSWToNW(Vector2i coord);
    void ScrollSWToSE(Vector2i coord);
    void ScrollSWToNE(Vector2i coord);

    //   2 | SW
    //  ---+---
    //   1 | 0
    void ScrollSW(Vector2i coord) {
        ReAllocateIfNeeded(0, {coord.x, coord.y + kRegionSize});
        ReAllocateIfNeeded(1, {coord.x - kRegionSize, coord.y + kRegionSize});
        ReAllocateIfNeeded(2, {coord.x - kRegionSize, coord.y});
    }

    void ScrollNEToSE(Vector2i coord);
    void ScrollNEToNW(Vector2i coord);
    void ScrollNEToSW(Vector2i coord);

    //    0 | 1
    //   ---+---
    //   NE | 2
    void ScrollNE(Vector2i coord) {
        ReAllocateIfNeeded(0, {coord.x, coord.y - kRegionSize});
        ReAllocateIfNeeded(1, {coord.x + kRegionSize, coord.y - kRegionSize});
        ReAllocateIfNeeded(2, {coord.x + kRegionSize, coord.y});
    }

    void ScrollNWToSW(Vector2i coord);
    void ScrollNWToNE(Vector2i coord);
    void ScrollNWToSE(Vector2i coord);

    //    0 | 1
    //   ---+---
    //    2 | NW
    void ScrollNW(Vector2i coord) {
        ReAllocateIfNeeded(0, {coord.x - kRegionSize, coord.y - kRegionSize});
        ReAllocateIfNeeded(1, {coord.x, coord.y - kRegionSize});
        ReAllocateIfNeeded(2, {coord.x - kRegionSize, coord.y});
    }

    DISALLOW_IMPLICIT_CONSTRUCTORS(EntitiesZone);

private:
    void ReAllocateIfNeeded(int index, Vector2i coord) {
        EntitiesRegion *region = *address(index);
        if (!region) {
            *address(index) = AllocateRegion(coord);
        } else if (coord.x != region->global_coord_.x && coord.y != region->global_coord_.y) {
            Finalize(region);
            region->global_coord_ = coord;
        }
    }

    EntitiesRegion *AllocateRegion(Vector2i coord) {
        DCHECK(!free_regions_.empty());
        EntitiesRegion *region = free_regions_.front();
        free_regions_.pop_front();
        region->global_coord_ = coord;
        return region;
    }

    void FreeIfNeeded(int index) {
        EntitiesRegion *region = *address(index);
        if (region) {
            Finalize(region);
            free_regions_.push_back(region);
            *address(index) = nullptr;
        }
    }

    void Finalize(EntitiesRegion *region) { /*TODO*/
    }

    EntitiesRegion **address(int index) { return index < 0 ? &region_ : &sibling_[index]; }

    std::list<EntitiesRegion *> free_regions_;
    EntitiesRegion *            region_ = nullptr;
    EntitiesRegion *            sibling_[4];
};  // class EntitiesZone

}  // namespace nyaa

#endif  // NYAA_GAME_ENTITIES_ZONE_H_
