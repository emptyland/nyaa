#include "system/random-region-system.h"
#include "component/zone-component.h"

namespace nyaa {

namespace sys {

void RandomRegionSystem::Update(com::RegionComponent *region) {
    // (1),  fill sky cube
    for (int i = 0; i < kTerrainMaxLevels; i++) { ::memset(region->floor(i), 0, sizeof(com::RegionComponent::Floor)); }

    // (2), fill surface level
    com::RegionComponent::Floor *surface = region->floor(kTerrainSurfaceLevel);
    for (int y = 0; y < kRegionSize; y++) {
        for (int x = 0; x < kRegionSize; x++) {
            if (::rand() & 0x3) {
                surface->cubes[x][y].set_kind(res::Cube::CUBE_DIRT_1);
            } else {
                surface->cubes[x][y].set_kind(res::Cube::CUBE_WALL_2);
            }
            surface->cubes[x][y].set_hardness(4);
        }
    }

    // (3) fill under ground
    for (int level = 0; level < kTerrainSurfaceLevel; level++) {
        for (int y = 0; y < kRegionSize; y++) {
            for (int x = 0; x < kRegionSize; x++) {
                if (::rand() & 0x1) {
                    surface->cubes[x][y].set_kind(res::Cube::CUBE_DIRT_1);
                } else {
                    surface->cubes[x][y].set_kind(res::Cube::CUBE_WALL_2);
                }
                surface->cubes[x][y].set_hardness(4);
            }
        }
    }
}

}  // namespace sys

}  // namespace nyaa