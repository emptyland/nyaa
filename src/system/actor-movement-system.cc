#include "system/actor-movement-system.h"
#include "component/movement-component.h"
#include "component/zone-component.h"

namespace nyaa {

namespace sys {

void ActorMovementSystem::Update(com::MovementComponent *movement, com::ZoneComponent *zone, float gravity,
                                 double delta, bool stop_after_move /*= true*/) {
    DCHECK_GE(movement->coord().z, 0);
    DCHECK_LT(movement->coord().z, kTerrainMaxLevels);
    
    com::CubeComponent *cube =
        zone->Cube(movement->round_down_coord_x(), movement->round_down_coord_y(), movement->round_down_coord_z());

    if (cube->kind() == res::Cube::CUBE_AIR) {
        if (movement->speed().z > 0) {
            movement->mutable_speed()->z -= gravity;
        } else {
            movement->mutable_speed()->z += -gravity;
        }
    } else {
        movement->mutable_speed()->z = 0;
    }

    // :format
    Vertex3f next;
    next.x = movement->coord().x + movement->speed().x * delta;
    next.y = movement->coord().y + movement->speed().y * delta;
    next.z = movement->coord().z + movement->speed().z * delta;

    //float ky = next.y / movement->coord().y, kz = next.z / movement->coord().z;

    cube = zone->Cube(next.x, next.y, next.z);
    //if (cube->kind() == res::Cube::CUBE_AIR) {
        movement->set_coord(next);
    //}

    // TODO:
    if (stop_after_move) {
        movement->mutable_speed()->x = 0;
        movement->mutable_speed()->y = 0;
    }
}

}  // namespace sys

}  // namespace nyaa
