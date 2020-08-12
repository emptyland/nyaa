#ifndef NYAA_ENTITY_ENTITY_H_
#define NYAA_ENTITY_ENTITY_H_

#include "game/identifiers.h"
#include "base/base.h"

namespace nyaa {

namespace entity {

class Entity {
public:
    Entity(EntityId id) : id_(id) {}

    DEF_VAL_PROP_RW(EntityId, id);

private:
    EntityId id_;
};  // class Entity

}  // namespace entity

}  // namespace nyaa

#endif  // NYAA_ENTITY_ENTITY_H_