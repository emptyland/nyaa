#ifndef NYAA_ENTITY_ENTITY_H_
#define NYAA_ENTITY_ENTITY_H_

#include "game/identifiers.h"
#include "game/vector.h"
#include "base/base.h"

namespace nyaa {

class EntityGrid;

namespace entity {

template <class T>
struct Class {
    inline static constexpr intptr_t id() { return reinterpret_cast<intptr_t>(&clazz_stub); }

    inline static const int clazz_stub = 0;
};  // struct Class

class Entity {
public:
    Entity() : Entity(EntityId::Of(0), 0) {}
    Entity(EntityId id, intptr_t clazz) : next_(this), prev_(this), id_(id), clazz_(clazz) {}
    virtual ~Entity() = default;

    DEF_VAL_GETTER(intptr_t, clazz);
    DEF_VAL_PROP_RW(EntityId, id);
    DEF_PTR_PROP_RW(Entity, next);
    DEF_PTR_PROP_RW(Entity, prev);
    DEF_PTR_PROP_RW(EntityGrid, grid);

    template <class T>
    bool Is() const {
        return clazz() == Class<T>::id();
    }

    template <class T>
    T *AsOrNull() {
        return Is<T>() ? static_cast<T *>(this) : nullptr;
    }

    virtual float ZOrder() const { return 0; }
    virtual float RadiusOrVolume() const { return 0; }
    virtual Vector3f Coord() const { return Vector3f{0, 0, 0}; };

    friend class ::nyaa::EntityGrid;

    DISALLOW_IMPLICIT_CONSTRUCTORS(Entity);

private:
    Entity *next_;
    Entity *prev_;

    EntityId    id_;
    intptr_t    clazz_;
    EntityGrid *grid_ = nullptr;
};  // class Entity

}  // namespace entity

}  // namespace nyaa

#endif  // NYAA_ENTITY_ENTITY_H_