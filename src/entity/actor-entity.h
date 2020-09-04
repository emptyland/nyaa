#pragma once
#ifndef NYAA_ENTITY_ACTOR_ENTITY_H_
#define NYAA_ENTITY_ACTOR_ENTITY_H_

#include "component/ai-state-component.h"
#include "component/avatar-component.h"
#include "component/movement-component.h"
#include "component/property-components.h"
#include "entity/entity.h"

namespace nyaa {
namespace res {
class Actor;
}  // namespace res

namespace entity {

class ActorEntity final : public Entity {
public:
    ActorEntity(EntityId id, ResourceId actor_id);
    ~ActorEntity() final = default;

    DEF_PTR_GETTER(const res::Actor, def);
    DEF_VAL_PROP_RM(com::AvatarComponent, avatar);
    DEF_VAL_PROP_RM(com::MovementComponent, movement);
    DEF_VAL_PROP_RM(com::AIStateComponent, ai_state);
    DEF_VAL_PROP_RM(com::CorePropertiesComponent, core);
    DEF_VAL_PROP_RM(com::CorePropertiesComponent, core_base);
    DEF_VAL_PROP_RM(com::NPCNaturePropertiesComponent, nature);

    void set_spawn_point(const Vector3f& point) { nature_.set_spawn_point(point); }

    float ZOrder() const final { return -movement().coord().y; }

    float RadiusOrVolume() const final { return 0.5; }

    DISALLOW_IMPLICIT_CONSTRUCTORS(ActorEntity);

private:
    const res::Actor* def_;

    com::AvatarComponent              avatar_;
    com::MovementComponent            movement_;
    com::AIStateComponent             ai_state_;
    com::StatusPropertiesComponent    status_;
    com::CorePropertiesComponent      core_;
    com::CorePropertiesComponent      core_base_;
    com::NPCNaturePropertiesComponent nature_;
};  // class ActorEntity

}  // namespace entity

}  // namespace nyaa

#endif  // NYAA_ENTITY_ACTOR_ENTITY_H_