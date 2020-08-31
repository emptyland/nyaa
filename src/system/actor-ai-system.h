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

namespace sys {

class ActorAISystem {
public:
    void Update(com::AIStateComponent * ai_state,  // :format
                com::MovementComponent *movement,  // :format
                com::ZoneComponent *    zone,      // :format
                double                  delta);

private:
};  // class ActorAISystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_ACTOR_AI_SYSTEM_H_