#include "system/actor-movement-system.h"
#include "component/movement-component.h"
#include "component/zone-component.h"

#define FLOOR_XYZ (::floorf(x) + 1), (::floorf(y) + 1), (::floorf(z))

namespace nyaa {

namespace sys {

void ActorMovementSystem::Update(com::MovementComponent *movement, com::ZoneComponent *zone, float gravity,
                                 double delta, bool stop_after_move /*= true*/) {
    // if (movement->coord().z < 0) { movement->mutable_coord()->z = 0; }
    DCHECK_GE(movement->coord().z, 0);
    DCHECK_LT(movement->coord().z, kTerrainMaxLevels);

    com::CubeComponent *cube = zone->Cube(movement->round_down_coord_x() + 1, movement->round_down_coord_y() + 1,
                                          movement->round_down_coord_z() + 1);

    if (cube->kind() == res::Cube::CUBE_AIR) {
        movement->mutable_speed()->z -= gravity;
    } else {
        if (movement->speed().z < 0) { movement->mutable_speed()->z = 0; }
    }

    Vertex3f next;
    next.x = movement->coord().x + movement->speed().x * delta;
    next.y = movement->coord().y + movement->speed().y * delta;
    next.z = movement->coord().z + movement->speed().z * delta;
    if (next.z < 0) { next.z = 0; }
    if (next.z >= kTerrainMaxLevels) { next.z = kTerrainMaxLevels - 1; }

    int i = next.x + 0.75;
    int j = next.y + 1;
    cube = zone->Cube(i, j, ceil(next.z) + 1);
    // DLOG(INFO) << "next: (" << next.x << ", " << next.y << ", " << next.z << ")";
    // DLOG(INFO) << "cube: " << cube->kind() << ":" << cube->hardness();
    if (cube->kind() == res::Cube::CUBE_AIR) {
        movement->set_coord(next);
    }
    
    if (stop_after_move) {
        movement->mutable_speed()->x = 0;
        movement->mutable_speed()->y = 0;
    }
}

}  // namespace sys

}  // namespace nyaa
