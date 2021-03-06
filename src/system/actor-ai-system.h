#pragma once
#ifndef NYAA_ACTOR_AI_SYSTEM_H_
#define NYAA_ACTOR_AI_SYSTEM_H_

#include "system/system.h"

namespace nyaa {
namespace com {
class MovementComponent;
class NPCNaturePropertiesComponent;
class ZoneComponent;
class AIStateComponent;
}  // namespace com
class EntityGridDB;
namespace sys {

class ActorAISystem final : public base::ArenaObject {
public:
    ActorAISystem() = default;

    void Update(com::AIStateComponent *            ai_state,  // :format
                com::MovementComponent *           movement,  // :format
                com::NPCNaturePropertiesComponent *nature_properties,
                com::ZoneComponent *               zone,       // :format
                EntityGridDB *                     entity_db,  // :format
                double                             delta);

    DISALLOW_IMPLICIT_CONSTRUCTORS(ActorAISystem);

private:
};  // class ActorAISystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_ACTOR_AI_SYSTEM_H_