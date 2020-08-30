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

    com::CubeComponent *cube = zone->Cube(movement->coord().x + 0.5, movement->round_down_coord_y(),
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

    bool stop = false;
    if (movement->speed().x != 0) {
        float d = movement->speed().x < 0 ? -1 : +1;
        for (int i = -2; i < 1 && !stop; i++) {
            float x = static_cast<int>(next.x + d);
            float y = static_cast<int>(next.y + i);
            if (cube = zone->Cube(x, y, ceil(next.z) + 1); !cube->IsAir()) {
                stop = IsIntersect(body, {x, y, 1, 1});
            }
        }
    }

    if (movement->speed().y != 0) {
        float d = movement->speed().y < 0 ? -0.2 : +0.2;
        for (int i = -1; i < 2 && !stop; i++) {
            float x = static_cast<int>(next.x + i);
            float y = static_cast<int>(next.y + d);
            if (cube = zone->Cube(x, y, ceil(next.z) + 1); !cube->IsAir()) {
                stop = IsIntersect(body, {x, y, -1, 1});
            }
        }
    }

    if (!stop) { movement->set_coord(next); }

    // int i = next.x;
    // int j = next.y;
    // cube = zone->Cube(i, j, ceil(next.z) + 1);
    // DLOG(INFO) << "next: (" << next.x << ", " << next.y << ", " << next.z << ")";
    // DLOG(INFO) << "cube: " << cube->kind() << ":" << cube->hardness();
    // if (cube->kind() == res::Cube::CUBE_AIR) {
    //     movement->set_coord(next);
    // }

    if (stop_after_move) {
        movement->mutable_speed()->x = 0;
        movement->mutable_speed()->y = 0;
    }
}

}  // namespace sys

}  // namespace nyaa
