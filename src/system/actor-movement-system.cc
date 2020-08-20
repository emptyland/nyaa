#include "system/actor-movement-system.h"
#include "component/movement-component.h"
#include "component/zone-component.h"

#define FLOOR_XYZ (::floorf(x) + 1), (::floorf(y) + 1), (::floorf(z))

namespace nyaa {

namespace sys {

float Nearf(float n) {
    float i = ceilf(n);
    float j = floorf(n);
    return fabsf(ceilf(n) - n) < fabsf(n - floorf(n)) ? i : j;
}

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

    // DLOG(INFO) << "Speed: (" << movement->speed().x << ", " << movement->speed().y << ", " << movement->speed().z
    //            << ")";

    // :format
    Vertex3f next;
    next.x = movement->coord().x + movement->speed().x * delta;
    next.y = movement->coord().y + movement->speed().y * delta;
    next.z = movement->coord().z + movement->speed().z * delta;
    if (next.z < 0) { next.z = 0; }
    if (next.z >= kTerrainMaxLevels) { next.z = kTerrainMaxLevels - 1; }

    int i = next.x + 1;
    int j = next.y + 1;
    cube = zone->Cube(i, j, ceil(next.z) + 1);
    DLOG(INFO) << "next: (" << next.x << ", " << next.y << ", " << next.z << ")";
    DLOG(INFO) << "cube: " << cube->kind() << ":" << cube->hardness();
    //if (cube->kind() == res::Cube::CUBE_AIR) {
        movement->set_coord(next);
    //}

    // bool near = false;
    // float dx = (next.x - movement->coord().x), dy = next.y - movement->coord().y, dz = next.z - movement->coord().z;
    // if (dx < 1 && dy < 1 && dz < 1) {
    //     if (cube = zone->Cube(floorf(next.x) + 1, floorf(next.y) + 1, floorf(next.z) + 1);
    //         cube->kind() == res::Cube::CUBE_AIR) {
    //         //DLOG(INFO) << "Probe (" << x << ", " << y << ", " << z << ") ok dy = " << dy;
    //         movement->set_coord(next);
    //         near = true;
    //     }
    // }

    // if (::fabsf(dx) > 0) {
    //     float step = dx < 0 ? -0.1 : 0.1;
    //     float ky = dy / dx, kz = dz / dx;
    //     for (float i = 0; ::fabsf(i) < ::fabsf(dx); i += step) {
    //         float x = movement->coord().x + i;
    //         float y = movement->coord().y + i * ky;
    //         float z = movement->coord().z + i * kz;

    //         movement->set_coord({x, y, z});
    //         DLOG(INFO) << "Probe... (" << x << ", " << y << ", " << z << ")";
    //         if (cube = zone->Cube(FLOOR_XYZ); cube->kind() != res::Cube::CUBE_AIR) {
    //             DLOG(INFO) << "Probe (" << x << ", " << y << ", " << z << ") ok dx = " << dx;
    //             //movement->set_coord({x, y, z});
    //             near = true;
    //             break;
    //         }
    //     }
    // } else if (::fabsf(dy) > 0) {
    //     float step = dy < 0 ? -0.1 : 0.1;
    //     float kx = dx / dy, kz = dz / dy;
    //     for (float i = 0; ::fabsf(i) < ::fabsf(dy); i += step) {
    //         float x = movement->coord().x + i * kx;
    //         float y = movement->coord().y + i;
    //         float z = movement->coord().z + i * kz;
    //         movement->set_coord({x, y, z});
    //         DLOG(INFO) << "Probe... (" << x << ", " << y << ", " << z << ")";
    //         if (cube = zone->Cube(FLOOR_XYZ); cube->kind() != res::Cube::CUBE_AIR) {
    //             DLOG(INFO) << "Probe (" << x << ", " << y << ", " << z << ") ok dy = " << dy;
    //             //movement->set_coord({x, y, z});
    //             near = true;
    //             break;
    //         }
    //     }
    // }

    // if (!near) { movement->set_coord(next); }

    // TODO:
    if (stop_after_move) {
        movement->mutable_speed()->x = 0;
        movement->mutable_speed()->y = 0;
    }
}

}  // namespace sys

}  // namespace nyaa
