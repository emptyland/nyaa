#ifndef NYAA_ENTITY_ENTITY_H_
#define NYAA_ENTITY_ENTITY_H_

#include "game/identifiers.h"
#include "base/base.h"

namespace nyaa {

class EntitiesGrid;

namespace entity {

class Entity {
public:
    Entity(EntityId id) : next_(this), prev_(this), id_(id) {}

    DEF_VAL_PROP_RW(EntityId, id);
    DEF_PTR_PROP_RW(Entity, next);
    DEF_PTR_PROP_RW(Entity, prev);
    DEF_PTR_PROP_RW(EntitiesGrid, grid);

    virtual float ZOrder() { return 0; }

    friend class ::nyaa::EntitiesGrid;

    DISALLOW_IMPLICIT_CONSTRUCTORS(Entity);
private:
    Entity *next_;
    Entity *prev_;

    EntityId      id_;
    EntitiesGrid *grid_ = nullptr;
};  // class Entity

}  // namespace entity

}  // namespace nyaa

#endif  // NYAA_ENTITY_ENTITY_H_