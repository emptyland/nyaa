#pragma once
#ifndef NYAA_ENTITY_ACTOR_ENTITY_H_
#define NYAA_ENTITY_ACTOR_ENTITY_H_

#include "component/ai-state-component.h"
#include "component/avatar-component.h"
#include "component/movement-component.h"
#include "entity/entity.h"

namespace nyaa {
namespace res {
class Actor;
}  // namespace res

namespace entity {

// class NatureComponent {
// public:

// private:
//     int yi_;
//     int camp_;
// };

class ActorEntity final : public Entity {
public:
    ActorEntity(EntityId id, ResourceId actor_id);
    ~ActorEntity() final = default;

    DEF_PTR_GETTER(const res::Actor, def);
    DEF_VAL_PROP_RM(com::AvatarComponent, avatar);
    DEF_VAL_PROP_RM(com::MovementComponent, movement);
    DEF_VAL_PROP_RM(com::AIStateComponent, ai_state);

    //const char *name() const { return def()->name(); }

    float ZOrder() const final { return -movement().coord().y; }

    DISALLOW_IMPLICIT_CONSTRUCTORS(ActorEntity);

private:
    const res::Actor* def_;

    com::AvatarComponent   avatar_;
    com::MovementComponent movement_;
    com::AIStateComponent  ai_state_;
};  // class ActorEntity

}  // namespace entity

}  // namespace nyaa

#endif  // NYAA_ENTITY_ACTOR_ENTITY_H_