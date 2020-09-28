#include "system/zone-loading-system.h"
#include "system/random-zone-system.h"
#include "component/zone-component.h"
#include "game/game.h"

namespace nyaa {

namespace sys {

void ZoneLoadingSystem::Update(com::ZoneComponent *zone) {
    Vector2i coord{
        zone->viewport().round_down_center_x(),
        zone->viewport().round_down_center_y(),
    };
    if (!zone->center()) {
        zone->set_region(1, 1, LoadRegion(coord.x / kRegionSize, coord.y / kRegionSize));
    }

    zone->UpdateViewportCoord();
}

com::RegionComponent *ZoneLoadingSystem::LoadRegion(int x, int y) {
    com::RegionComponent *region = new com::RegionComponent();
    region->set_global_coord({x, y});

    // TODO:
    System::This()->random_zone()->Update(region);

    return region;
}

}  // namespace sys

}  // namespace nyaa
