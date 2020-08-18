#include "system/actor-movement-system.h"
#include "component/movement-component.h"
#include "component/zone-component.h"

namespace nyaa {

namespace sys {

void ActorMovementSystem::Update(com::MovementComponent *movement, com::ZoneComponent *zone, double delta,
                                 bool stop_after_move /*= true*/) {
    // :format
    Vertex3f next;
    next.x = movement->coord().x + movement->speed().x * delta;
    next.y = movement->coord().y + movement->speed().y * delta;
    next.z = movement->coord().z + movement->speed().z * delta;

    // TODO:
    if (stop_after_move) { ::memset(movement->mutable_speed(), 0, sizeof(*movement->mutable_speed())); }
}

}  // namespace sys

}  // namespace nyaa
