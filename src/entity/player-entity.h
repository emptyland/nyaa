#pragma once
#ifndef NYAA_ENTITY_PLAYER_ENTITY_H_
#define NYAA_ENTITY_PLAYER_ENTITY_H_

#include "component/avatar-component.h"
#include "component/movement-component.h"
#include "entity/entity.h"

namespace nyaa {
namespace entity {

class PlayerEntity : public Entity {
public:
    PlayerEntity(EntityId id, ResourceId avatar_id);

    DEF_VAL_PROP_RM(com::AvatarComponent, avatar);
    DEF_VAL_PROP_RM(com::MovementComponent, movement);

private:
    com::AvatarComponent   avatar_;
    com::MovementComponent movement_;
};  // class PlayerEntity

}  // namespace entity

}  // namespace nyaa

#endif  // NYAA_ENTITY_PLAYER_ENTITY_H_