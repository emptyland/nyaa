#include "system/random-zone-system.h"
#include "component/zone-component.h"

namespace nyaa {

namespace sys {

void RandomZoneSystem::Update(com::ZoneComponent *zone) {
    if (!zone->region()) {
        zone->set_region(new com::RegionComponent());
        Update(zone->region());
    }

    switch (zone->UpdateViewportCoord(zone->viewport().center_coord())) {
        case com::ZoneComponent::kE:
        case com::ZoneComponent::kS:
        case com::ZoneComponent::kW:
        case com::ZoneComponent::kN: break;

        case com::ZoneComponent::kSE:
        case com::ZoneComponent::kSW:
        case com::ZoneComponent::kNE:
        case com::ZoneComponent::kNW: break;

        case com::ZoneComponent::kNone: break;
        default: NOREACHED(); break;
    }
}

void RandomZoneSystem::Update(com::RegionComponent *region) {
    // (1),  fill sky cube
    for (int i = 0; i < kTerrainMaxLevels; i++) { ::memset(region->floor(i), 0, sizeof(com::RegionComponent::Floor)); }

    // (2), fill surface level
    com::RegionComponent::Floor *surface = region->floor(kTerrainSurfaceLevel);
    for (int y = 0; y < kRegionSize; y++) {
        for (int x = 0; x < kRegionSize; x++) {
            int seed = ::rand() & 0x7;
            switch (seed) {
                case 0: surface->cubes[x][y].set_kind(res::Cube::CUBE_DIRT_1); break;
                case 1: surface->cubes[x][y].set_kind(res::Cube::CUBE_STONE_1); break;
                case 2: surface->cubes[x][y].set_kind(res::Cube::CUBE_DIRT_2); break;
                case 3: surface->cubes[x][y].set_kind(res::Cube::CUBE_GRASS_2); break;
                default: surface->cubes[x][y].set_kind(res::Cube::CUBE_GRASS_1); break;
            }
            surface->cubes[x][y].set_hardness(4);
        }
    }

    // (3) fill terrain
    surface = region->floor(kTerrainSurfaceLevel + 1);
    for (int y = 0; y < kRegionSize; y++) {
        for (int x = 0; x < kRegionSize; x++) {
            int seed = ::rand() & 0x31;
            switch (seed) {
                //case 0: surface->cubes[x][y].set_kind(res::Cube::CUBE_STONE_1); break;
                case 1: surface->cubes[x][y].set_kind(res::Cube::CUBE_DIRT_1); break;
                default: surface->cubes[x][y].set_kind(res::Cube::CUBE_AIR); break;
            }
            surface->cubes[x][y].set_hardness(4);
        }
    }

    // (4) fill under ground
    for (int level = 0; level < kTerrainSurfaceLevel; level++) {
        surface = region->floor(level);
        for (int y = 0; y < kRegionSize; y++) {
            for (int x = 0; x < kRegionSize; x++) {
                if (::rand() & 0x1) {
                    surface->cubes[x][y].set_kind(res::Cube::CUBE_DIRT_1);
                } else {
                    surface->cubes[x][y].set_kind(res::Cube::CUBE_DIRT_2);
                }
                surface->cubes[x][y].set_hardness(4);
            }
        }
    }
}

}  // namespace sys

}  // namespace nyaa
