#pragma once
#ifndef NYAA_SYSTEM_ACTOR_MOVEMENT_SYSTEM_H_
#define NYAA_SYSTEM_ACTOR_MOVEMENT_SYSTEM_H_

#include "base/base.h"

namespace nyaa {

namespace com {
class MovementComponent;
class ZoneComponent;
}  // namespace com

namespace sys {

class ActorMovementSystem {
public:
    ActorMovementSystem() = default;

    void Update(com::MovementComponent *movement, com::ZoneComponent *zone, double delta, bool stop_after_move = true);

    DISALLOW_IMPLICIT_CONSTRUCTORS(ActorMovementSystem);

private:
};  // class ActorMovementSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_SYSTEM_ACTOR_MOVEMENT_SYSTEM_H_