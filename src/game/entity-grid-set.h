#pragma once
#ifndef NYAA_GAME_ENTITY_GRID_SET_H_
#define NYAA_GAME_ENTITY_GRID_SET_H_

#include "entity/entity.h"
#include "game/constants.h"
#include "game/vector.h"
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
class ActorEntity;
}  // namespace entity
namespace com {
class ViewportComponent;
}  // namespace com

class EntityGrid {
public:
    EntityGrid() = default;

    void Enter(entity::Entity *obj);

    void Exit(entity::Entity *obj) {
        if (obj->grid() != this) { return; }
        QUEUE_REMOVE(obj);
        obj->set_grid(nullptr);
    }

    class iterator {
    public:
        iterator(entity::Entity *node) : node_(node) {}
        void operator++() { node_ = node_->next(); }
        void operator++(int) { node_ = node_->next(); }
        void operator--() { node_ = node_->prev(); }
        void operator--(int) { node_ = node_->prev(); }
        bool operator==(const iterator &iter) const { return iter.node_ == node_; }
        bool operator!=(const iterator &iter) const { return iter.node_ != node_; }

        entity::Entity *operator*() const { return node_; }

    private:
        entity::Entity *node_;
    };  // class iterator

    class const_iterator {
    public:
        const_iterator(const entity::Entity *node) : node_(node) {}
        void operator++() { node_ = node_->next(); }
        void operator++(int) { node_ = node_->next(); }
        void operator--() { node_ = node_->prev(); }
        void operator--(int) { node_ = node_->prev(); }
        bool operator==(const const_iterator &iter) const { return iter.node_ == node_; }
        bool operator!=(const const_iterator &iter) const { return iter.node_ != node_; }

        const entity::Entity *operator*() const { return node_; }

    private:
        const entity::Entity *node_;
    };  // class const_iterator

    iterator begin() { return iterator(entities_dummy_.next()); }
    iterator end() { return iterator(&entities_dummy_); }

    const_iterator begin() const { return const_iterator(entities_dummy_.next()); }
    const_iterator end() const { return const_iterator(&entities_dummy_); }

    friend class EntityGridSet;

private:
    entity::Entity *dummy() { return &entities_dummy_; }

    Vector2i       coord_;
    entity::Entity entities_dummy_;
};  // class EntityGrid

class EntityGridSet {
public:
    EntityGridSet();
    ~EntityGridSet();

    DEF_PTR_GETTER(entity::PlayerEntity, player);
    const EntityGrid *empty_grid() const { return &empty_grid_; }

    EntityGrid *grid(int x, int y) {
        DCHECK_GE(x, 0);
        DCHECK_LT(x, kRegionSize);
        DCHECK_GE(y, 0);
        DCHECK_LT(y, kRegionSize);
        return &grids_[x][y];
    }

    EntityGrid *ViewGrid(const com::ViewportComponent &viewport, int x, int y);

    void UpdatePlayer(entity::PlayerEntity *obj);
    void UpdatePlant(entity::PlantEntity *obj);
    void UpdateActor(entity::ActorEntity *obj);

    void Remove(entity::Entity *obj) {
        entities_.erase(obj->id());
        if (obj->grid()) { obj->grid()->Exit(obj); }
    }

    DISALLOW_IMPLICIT_CONSTRUCTORS(EntityGridSet);

private:
    void Update(const Vector3f &position, entity::Entity *obj) {
        EntityGrid *g = grid(position.x, position.y);
        if (!obj->grid()) {
            g->Enter(obj);
            entities_[obj->id()] = obj;
        } else {
            obj->grid()->Exit(obj);
            g->Enter(obj);
        }
    }

    entity::PlayerEntity *                                     player_ = nullptr;
    std::unordered_map<EntityId, entity::Entity *, EntityHash> entities_;

    EntityGrid empty_grid_;
    EntityGrid grids_[kRegionSize][kRegionSize];
};  // class EntityGridSet

}  // namespace nyaa

#endif  // NYAA_GAME_ENTITY_GRID_SET_H_
