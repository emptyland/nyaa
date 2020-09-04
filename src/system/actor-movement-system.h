#pragma once
#ifndef NYAA_SYSTEM_ACTOR_MOVEMENT_SYSTEM_H_
#define NYAA_SYSTEM_ACTOR_MOVEMENT_SYSTEM_H_

#include "game/identifiers.h"
#include "base/base.h"

namespace nyaa {

namespace com {
class MovementComponent;
class ZoneComponent;
}  // namespace com

namespace sys {

class ImpactCheckingSystem;

class ActorMovementSystem {
public:
    ActorMovementSystem() = default;

    void Update(EntityId id, com::MovementComponent *movement, float gravity, ImpactCheckingSystem *impact,
                double delta);

    DISALLOW_IMPLICIT_CONSTRUCTORS(ActorMovementSystem);

private:
};  // class ActorMovementSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_SYSTEM_ACTOR_MOVEMENT_SYSTEM_H_