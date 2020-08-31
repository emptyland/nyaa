#pragma once
#ifndef NYAA_ACTOR_AI_SYSTEM_H_
#define NYAA_ACTOR_AI_SYSTEM_H_

#include "base/base.h"

namespace nyaa {
namespace com {
class MovementComponent;
class ZoneComponent;
class AIStateComponent;
}  // namespace com
class EntityGridDB;
namespace sys {

class ActorAISystem {
public:
    ActorAISystem() = default;

    void Update(com::AIStateComponent * ai_state,   // :format
                com::MovementComponent *movement,   // :format
                com::ZoneComponent *    zone,       // :format
                EntityGridDB *          entity_db,  // :format
                double                  delta);

    DISALLOW_IMPLICIT_CONSTRUCTORS(ActorAISystem);
private:
};  // class ActorAISystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_ACTOR_AI_SYSTEM_H_