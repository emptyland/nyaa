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
    LoadRegion(zone->mutable_region(), coord.x, coord.y);
}

com::RegionComponent *ZoneLoadingSystem::LoadRegion(com::RegionComponent *region, int x, int y) {
    region->set_global_coord({x, y});

    // TODO:
    System::This()->random_zone()->Update(region);

    return region;
}

}  // namespace sys

}  // namespace nyaa
