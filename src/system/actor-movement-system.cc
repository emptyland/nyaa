#include "system/actor-movement-system.h"
#include "component/movement-component.h"
#include "component/zone-component.h"

#define FLOOR_XYZ                                                                                   \
    (step < 0 ? ::floorf(x) - 1 : ::floorf(x) + 1), (step < 0 ? ::floorf(y) - 1 : ::floorf(y) + 1), \
        (step < 0 ? ::floorf(z) - 1 : ::floorf(z) + 1)

namespace nyaa {

namespace sys {

float Nearf(float n) {
    float i = ceilf(n);
    float j = floorf(n);
    return fabsf(ceilf(n) - n) < fabsf(n - floorf(n)) ? i : j;
}

void ActorMovementSystem::Update(com::MovementComponent *movement, com::ZoneComponent *zone, float gravity,
                                 double delta, bool stop_after_move /*= true*/) {
    //if (movement->coord().z < 0) { movement->mutable_coord()->z = 0; }
    DCHECK_GE(movement->coord().z, 0);
    DCHECK_LT(movement->coord().z, kTerrainMaxLevels);

    com::CubeComponent *cube =
        zone->Cube(movement->round_down_coord_x() + 1, movement->round_down_coord_y() + 1, movement->round_down_coord_z() + 1);

    if (cube->kind() == res::Cube::CUBE_AIR) {
        movement->mutable_speed()->z -= gravity;
    } else {
        if (movement->speed().z < 0) {
            movement->mutable_speed()->z = 0;
        }
    }

    DLOG(INFO) << "Speed: (" << movement->speed().x << ", " << movement->speed().y << ", " << movement->speed().z
               << ")";

    // :format
    Vertex3f next;
    next.x = movement->coord().x + movement->speed().x * delta;
    next.y = movement->coord().y + movement->speed().y * delta;
    next.z = movement->coord().z + movement->speed().z * delta;
    if (next.z < 0) { next.z = 0; }
    if (next.z >= kTerrainMaxLevels) { next.z = kTerrainMaxLevels - 1; }

    // bool near = false;

    // float dx = (next.x - movement->coord().x), dy = next.y - movement->coord().y, dz = next.z - movement->coord().z;

    // if (::fabsf(dx) > 0) {
    //     float step = dx < 0 ? -0.2 : 0.2;
    //     float ky = (next.y - movement->coord().y) / dx, kz = (next.z - movement->coord().z) / dx;
    //     for (float i = 0; ::fabsf(i) < ::fabsf(dx); i += step) {
    //         float x = movement->coord().x + i;
    //         float y = movement->coord().y + i * ky;
    //         float z = movement->coord().z + i * kz;

    //         if (cube = zone->Cube(FLOOR_XYZ); cube->kind() != res::Cube::CUBE_AIR) {
    //             // DLOG(INFO) << "Probe (" << x << ", " << y << ", " << z << ") ok";
    //             movement->set_coord({x, y, z});
    //             near = true;
    //         }
    //     }
    // } else if (::fabsf(dy) > 0) {
    //     float step = dy < 0 ? -0.2 : 0.2;
    //     float kx = (next.x - movement->coord().x) / dy, kz = (next.z - movement->coord().z) / dy;
    //     for (float i = 0; ::fabsf(i) < ::fabsf(dy); i += step) {
    //         float x = movement->coord().x + i * kx;
    //         float y = movement->coord().y + i;
    //         float z = movement->coord().z + i * kz;

    //         if (cube = zone->Cube(FLOOR_XYZ); cube->kind() != res::Cube::CUBE_AIR) {
    //             // DLOG(INFO) << "Probe (" << x << ", " << y << ", " << z << ") ok";
    //             movement->set_coord({x, y, z});
    //             near = true;
    //         }
    //     }
    // } else if (::fabsf(dz) > 0) {
    //     float step = dz < 0 ? -0.1 : 0.1;
    //     float kx = (next.x - movement->coord().x) / dz, ky = (next.y - movement->coord().y) / dz;
    //     for (float i = 0; ::fabsf(i) < ::fabsf(dz); i += step) {
    //         float x = movement->coord().x + i * kx;
    //         float y = movement->coord().y + i * ky;
    //         float z = movement->coord().z + i;

    //         if (cube = zone->Cube(FLOOR_XYZ); cube->kind() != res::Cube::CUBE_AIR) {
    //             DLOG(INFO) << "Probe (" << x << ", " << y << ", " << z << ") ok";
    //             movement->set_coord({x, y, z - step});
    //             near = true;
    //         }
    //     }
    // }

    // if (!near) { movement->set_coord(next); }

    movement->set_coord(next);

    // TODO:
    if (stop_after_move) {
        movement->mutable_speed()->x = 0;
        movement->mutable_speed()->y = 0;
    }
}

}  // namespace sys

}  // namespace nyaa
