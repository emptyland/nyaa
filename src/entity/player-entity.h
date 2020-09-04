#pragma once
#ifndef NYAA_ENTITY_PLAYER_ENTITY_H_
#define NYAA_ENTITY_PLAYER_ENTITY_H_

#include "component/avatar-component.h"
#include "component/movement-component.h"
#include "component/property-components.h"
#include "entity/entity.h"

namespace nyaa {

namespace entity {

class PlayerEntity : public Entity {
public:
    PlayerEntity(EntityId id, ResourceId avatar_id);

    DEF_VAL_PROP_RM(com::AvatarComponent, avatar);
    DEF_VAL_PROP_RM(com::MovementComponent, movement);
    DEF_VAL_PROP_RM(com::CorePropertiesComponent, core);
    DEF_VAL_PROP_RM(com::CorePropertiesComponent, core_base);
    DEF_VAL_PROP_RM(com::NaturePropertiesComponent, nature);

    float ZOrder() const final { return -movement().coord().y; }

    float RadiusOrVolume() const final { return 0.5; }

    DISALLOW_IMPLICIT_CONSTRUCTORS(PlayerEntity);

private:
    com::AvatarComponent           avatar_;
    com::MovementComponent         movement_;
    com::StatusPropertiesComponent status_;
    com::CorePropertiesComponent   core_;
    com::CorePropertiesComponent   core_base_;
    com::NaturePropertiesComponent nature_;
};  // class PlayerEntity

}  // namespace entity

}  // namespace nyaa

#endif  // NYAA_ENTITY_PLAYER_ENTITY_H_