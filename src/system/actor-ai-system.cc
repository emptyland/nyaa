#include "system/actor-ai-system.h"
#include "component/zone-component.h"
#include "component/movement-component.h"
#include "component/ai-state-component.h"
#include "component/property-components.h"
#include "game/entity-virtual-db.h"

namespace nyaa {

namespace sys {

void ActorAISystem::Update(com::AIStateComponent *            ai_state,           // :format
                           com::MovementComponent *           movement,           // :format
                           com::NPCNaturePropertiesComponent *nature_properties,  // :format
                           com::ZoneComponent *               zone,               // :format
                           EntityGridDB *                     entity_db,          // :format
                           double                             delta) {
    switch (ai_state->kind()) {
        case res::AI::NONE:
            // Do nothing
            break;
        case res::AI::STUPID:
            // Just jump
            if (movement->speed().z == 0) { movement->mutable_speed()->z = nature_properties->jump_speed(); }
            break;

        case res::AI::HAND_OFF: {
            // TODO:
        } break;

        case res::AI::FOLLOW: {
            // TODO:
        } break;
    }
}

}  // namespace sys

}  // namespace nyaa
