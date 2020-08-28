#include "system/random-zone-system.h"
#include "component/zone-component.h"
#include "resource/cube-library.h"
#include "resource/texture-library.h"
#include "game/game.h"

namespace nyaa {

namespace sys {

void RandomZoneSystem::Update(com::ZoneComponent *zone) {
    if (!zone->center()) {
        zone->set_region(1, 1, new com::RegionComponent());
        Update(zone->center());
    }

    zone->UpdateViewportCoord(zone->viewport().center_coord());
}

void RandomZoneSystem::Update(com::RegionComponent *region) {
    res::TextureLibrary *tex_lib = Game::This()->texture_lib();

    // (1),  fill sky cube
    for (int i = 0; i < kTerrainMaxLevels; i++) { ::memset(region->floor(i), 0, sizeof(com::RegionComponent::Floor)); }

    // (2), fill surface level
    com::RegionComponent::Floor *surface = region->floor(kTerrainSurfaceLevel);
    for (int y = 0; y < kRegionSize; y++) {
        for (int x = 0; x < kRegionSize; x++) {
            int seed = ::rand() & 0x7;
            switch (seed) {
                case 0: surface->cubes[x][y].set_kind(res::Cube::CUBE_DIRT_1); break;
                // case 1: surface->cubes[x][y].set_kind(res::Cube::CUBE_STONE_1); break;
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
            int seed = ::rand() & 0x1f;
            switch (seed) {
                // case 0: surface->cubes[x][y].set_kind(res::Cube::CUBE_STONE_1); break;
                case 0: surface->cubes[x][y].set_kind(res::Cube::CUBE_GRASS_2); break;
                case 1: surface->cubes[x][y].set_kind(res::Cube::CUBE_GRASS_1); break;
                case 2: {
                    size_t pos = region->plants_size();
                    region->mutable_plants()->resize(pos + 1);
                    com::PlantComponent *plant = region->plant(pos);
                    plant->set_position({x + 0.5f, y + 0.5f, kTerrainSurfaceLevel + 0.0f});
                    plant->set_tex(tex_lib->FindOrNull(ResourceId::Of((rand() & 0x1) ? 300000 : 300010)));
                } break;
                default: surface->cubes[x][y].set_kind(res::Cube::CUBE_AIR); break;
            }
            if (surface->cubes[x][y].kind() != res::Cube::CUBE_AIR) { surface->cubes[x][y].set_hardness(4); }
        }
    }

    // (4) fill under ground
    for (int level = 0; level < kTerrainSurfaceLevel; level++) {
        surface = region->floor(level);
        for (int y = 0; y < kRegionSize; y++) {
            for (int x = 0; x < kRegionSize; x++) {
                int seed = ::rand() & 0x3;
                switch (seed) {
                    case 0: surface->cubes[x][y].set_kind(res::Cube::CUBE_DIRT_1); break;
                    case 1: surface->cubes[x][y].set_kind(res::Cube::CUBE_STONE_1); break;
                    default: surface->cubes[x][y].set_kind(res::Cube::CUBE_DIRT_2); break;
                }
                surface->cubes[x][y].set_hardness(4);
            }
        }
    }
}

}  // namespace sys

}  // namespace nyaa
