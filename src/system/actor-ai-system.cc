#include "system/actor-ai-system.h"
#include "component/zone-component.h"
#include "component/movement-component.h"
#include "component/ai-state-component.h"
#include "game/entity-virtual-db.h"

namespace nyaa {

namespace sys {

void ActorAISystem::Update(com::AIStateComponent * ai_state,   // :format
                           com::MovementComponent *movement,   // :format
                           com::ZoneComponent *    zone,       // :format
                           EntityGridDB *          entity_db,  // :format
                           double                  delta) {
    // TODO:
}

}  // namespace sys

}  // namespace nyaa
