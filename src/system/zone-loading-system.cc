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
        ReplaceRegionIfNeeded(zone, zone->region(1, 1));
        if (listener()) { listener()->DidLoadRegion(-1, zone->center()); }
    }

    if (zone->center()->AtEast(coord.x, coord.y)) {
        ReplaceRegionIfNeeded(zone, zone->region(2, 1));
    } else if (zone->center()->AtSouth(coord.x, coord.y)) {
        ReplaceRegionIfNeeded(zone, zone->region(1, 2));
    } else if (zone->center()->AtWest(coord.x, coord.y)) {
        ReplaceRegionIfNeeded(zone, zone->region(0, 1));
    } else if (zone->center()->AtNorth(coord.x, coord.y)) {
        ReplaceRegionIfNeeded(zone, zone->region(1, 0));
    } else if (zone->center()->AtSouthEast(coord.x, coord.y)) {
        ReplaceRegionIfNeeded(zone, zone->region(2, 2));
    } else if (zone->center()->AtSouthWest(coord.x, coord.y)) {
        ReplaceRegionIfNeeded(zone, zone->region(0, 2));
    } else if (zone->center()->AtNorthEast(coord.x, coord.y)) {
        ReplaceRegionIfNeeded(zone, zone->region(2, 0));
    } else if (zone->center()->AtNorthWest(coord.x, coord.y)) {
        ReplaceRegionIfNeeded(zone, zone->region(0, 0));
    } else {
        DCHECK(zone->center()->AtBound(coord.x, coord.y));
    }
    zone->UpdateViewportCoord();
}

void ZoneLoadingSystem::ReplaceRegionIfNeeded(com::ZoneComponent *zone, com::RegionComponent *center) {
    zone->ClearRegions();
    zone->set_region(1, 1, DCHECK_NOTNULL(center));
    int x = center->global_coord().x - kRegionSize;
    int y = center->global_coord().y - kRegionSize;
    if (x >= 0 && y >= 0) {
        zone->set_region(0, 0, LoadRegion(x, y));
    }
    x = center->global_coord().x;
    y = center->global_coord().y - kRegionSize;
    if (x >= 0 && y >= 0) {
        zone->set_region(1, 0, LoadRegion(x, y));
    }
    x = center->global_coord().x + kRegionSize;
    y = center->global_coord().y - kRegionSize;
    if (x >= 0 && y >= 0) {
        zone->set_region(2, 0, LoadRegion(x, y));
    }
    x = center->global_coord().x - kRegionSize;
    y = center->global_coord().y;
    if (x >= 0 && y >= 0) {
        zone->set_region(0, 1, LoadRegion(x, y));
    }
    x = center->global_coord().x + kRegionSize;
    y = center->global_coord().y;
    if (x >= 0 && y >= 0) {
        zone->set_region(2, 1, LoadRegion(x, y));
    }
    x = center->global_coord().x - kRegionSize;
    y = center->global_coord().y + kRegionSize;
    if (x >= 0 && y >= 0) {
        zone->set_region(0, 2, LoadRegion(x, y));
    }
    x = center->global_coord().x;
    y = center->global_coord().y + kRegionSize;
    if (x >= 0 && y >= 0) {
        zone->set_region(1, 2, LoadRegion(x, y));
    }
    x = center->global_coord().x + kRegionSize;
    y = center->global_coord().y + kRegionSize;
    if (x >= 0 && y >= 0) {
        zone->set_region(2, 2, LoadRegion(x, y));
    }
}

com::RegionComponent *ZoneLoadingSystem::LoadRegion(int x, int y) {
    com::RegionComponent *region = new com::RegionComponent();
    region->set_global_coord({x, y});

    // TODO:
    Game::This()->random_zone()->Update(region);

    return region;
}

}  // namespace sys

}  // namespace nyaa
