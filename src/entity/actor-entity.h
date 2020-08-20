#pragma once
#ifndef NYAA_ENTITY_ACTOR_ENTITY_H_
#define NYAA_ENTITY_ACTOR_ENTITY_H_

#include "component/avatar-component.h"
#include "component/movement-component.h"
#include "entity/entity.h"

namespace nyaa {
// :format
namespace entity {

class ActorEntity final : public Entity {
public:
    ActorEntity(EntityId id, ResourceId avatar_id);

    DEF_VAL_PROP_RM(com::AvatarComponent, avatar);
    DEF_VAL_PROP_RM(com::MovementComponent, movement);

    DISALLOW_IMPLICIT_CONSTRUCTORS(ActorEntity);
private:
    com::AvatarComponent   avatar_;
    com::MovementComponent movement_;
}; // class ActorEntity

}  // namespace entity

}  // namespace nyaa

#endif  // NYAA_ENTITY_ACTOR_ENTITY_H_