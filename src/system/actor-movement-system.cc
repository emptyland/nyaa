#include "system/actor-movement-system.h"
#include "system/impact-checking-system.h"
#include "component/movement-component.h"
#include "component/zone-component.h"

#define FLOOR_XYZ (::floorf(x) + 1), (::floorf(y) + 1), (::floorf(z))

namespace nyaa {

namespace sys {

void ActorMovementSystem::Update(com::MovementComponent *movement, float gravity, ImpactCheckingSystem *impact,
                                 double delta) {
    DCHECK_GE(movement->coord().z, 0);
    DCHECK_LT(movement->coord().z, kTerrainMaxLevels);

    com::CubeComponent *cube = impact->zone()->Cube(movement->coord().x + 0.5, movement->round_down_coord_y(),
                                                    movement->round_down_coord_z() + 1);

    if (cube->kind() == res::Cube::CUBE_AIR) {
        movement->mutable_speed()->z -= gravity;
    } else {
        if (movement->speed().z < 0) {
            movement->mutable_speed()->z = 0;
            movement->mutable_coord()->z = movement->round_down_coord_z() + 1;
        }
    }

    Vector3f next;
    next.x = movement->coord().x + movement->speed().x * delta;
    next.y = movement->coord().y + movement->speed().y * delta;
    next.z = movement->coord().z + movement->speed().z * delta;
    if (next.z < 0) { next.z = 0; }
    if (next.z >= kTerrainMaxLevels - 1) { next.z = kTerrainMaxLevels - 2; }

    Boundf body;
    body.x = next.x;
    body.y = next.y;
    body.w = 0.5;
    body.h = 0.2;

    bool stop = impact->PredictTest(body, next, movement->speed());
    if (!stop) { movement->set_coord(next); }
}

}  // namespace sys

}  // namespace nyaa
