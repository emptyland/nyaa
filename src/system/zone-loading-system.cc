#include "system/zone-loading-system.h"
#include "system/random-zone-system.h"
#include "component/zone-component.h"
#include "game/game.h"

namespace nyaa {

namespace sys {

void ZoneLoadingSystem::Update(com::ZoneComponent *zone) {
    Vertex2i viewport_center{
        zone->viewport().round_down_center_x(),
        zone->viewport().round_down_center_y(),
    };
    if (!zone->region()) {
        zone->set_region(LoadRegion(viewport_center.x / kRegionSize, viewport_center.y / kRegionSize));
    }

    zone->UpdateViewportCoord();
    switch (zone->want()) {
        case com::ZoneComponent::kE: {
            if (zone->region()->AtEast(viewport_center.x, viewport_center.y)) {
                com::RegionComponent *east = zone->region();
                zone->set_region(zone->sibling(0));
                ReplaceRegionIfNeeded(zone, east, -1, 1, 0);

                zone->set_sibling(0, east);
                ReplaceRegionIfNeeded(zone, east, 0, 0, 0);

                zone->UpdateViewportCoord();
                DCHECK_EQ(com::ZoneComponent::kW, zone->want());
            } else {
                ReplaceRegionIfNeeded(zone, zone->region(), 0, 1, 0);
            }
            zone->set_sibling(1, nullptr);
            zone->set_sibling(2, nullptr);
            DCHECK(zone->region()->AtBound(viewport_center.x, viewport_center.y));
        } break;
        case com::ZoneComponent::kS: {
            if (zone->region()->AtSouth(viewport_center.x, viewport_center.y)) {
                com::RegionComponent *south = zone->region();
                zone->set_region(zone->sibling(0));
                ReplaceRegionIfNeeded(zone, south, -1, 0, 1);

                zone->set_sibling(0, south);
                ReplaceRegionIfNeeded(zone, south, 0, 0, 0);

                zone->UpdateViewportCoord();
                DCHECK_EQ(com::ZoneComponent::kN, zone->want());
            } else {
                ReplaceRegionIfNeeded(zone, zone->region(), 0, 0, 1);
            }
            zone->set_sibling(1, nullptr);
            zone->set_sibling(2, nullptr);
            DCHECK(zone->region()->AtBound(viewport_center.x, viewport_center.y));
        } break;
        case com::ZoneComponent::kW: {
            if (zone->region()->AtWest(viewport_center.x, viewport_center.y)) {
                com::RegionComponent *west = zone->region();
                zone->set_region(zone->sibling(0));
                ReplaceRegionIfNeeded(zone, west, -1, -1, 0);

                zone->set_sibling(0, west);
                ReplaceRegionIfNeeded(zone, west, 0, 0, 0);

                zone->UpdateViewportCoord();
                DCHECK_EQ(com::ZoneComponent::kE, zone->want());
            } else {
                ReplaceRegionIfNeeded(zone, zone->region(), 0, -1, 0);
            }
            zone->set_sibling(1, nullptr);
            zone->set_sibling(2, nullptr);
            DCHECK(zone->region()->AtBound(viewport_center.x, viewport_center.y));
        } break;
        case com::ZoneComponent::kN: {
            if (zone->region()->AtNorth(viewport_center.x, viewport_center.y)) {
                com::RegionComponent *north = zone->region();
                zone->set_region(zone->sibling(0));
                ReplaceRegionIfNeeded(zone, north, -1, 0, -1);

                zone->set_sibling(0, north);
                ReplaceRegionIfNeeded(zone, north, 0, 0, 0);

                zone->UpdateViewportCoord();
                DCHECK_EQ(com::ZoneComponent::kS, zone->want());
            } else {
                ReplaceRegionIfNeeded(zone, zone->region(), 0, 0, -1);
            }
            zone->set_sibling(1, nullptr);
            zone->set_sibling(2, nullptr);
            DCHECK(zone->region()->AtBound(viewport_center.x, viewport_center.y));
        } break;
        //
        //     SE | [0]
        // -------+------
        //    [2] | [1]
        case com::ZoneComponent::kSE: {
            if (zone->region()->AtEast(viewport_center.x, viewport_center.y)) {
                //    [SE->0] | [0->SW]
                // -----------+---------
                //        [2] | [1]
                com::RegionComponent *se = zone->region();
                zone->set_region(zone->sibling(0));
                ReplaceRegionIfNeeded(zone, se, -1, 1, 0);

                zone->set_sibling(0, se);
                ReplaceRegionIfNeeded(zone, se, 0, 0, 0);

                ReplaceRegionIfNeeded(zone, se, 1, 1, 1);

                ReplaceRegionIfNeeded(zone, se, 2, 0, 1);

                zone->UpdateViewportCoord();
                DCHECK_EQ(com::ZoneComponent::kSW, zone->want());
            } else if (zone->region()->AtSouth(viewport_center.x, viewport_center.y)) {
                //    [SE->0]  | [0->1]
                // ------------+------
                //    [2->NE]  | [1->2]
                com::RegionComponent *se = zone->region();
                zone->set_region(zone->sibling(2));
                ReplaceRegionIfNeeded(zone, se, -1, 0, 1);

                zone->set_sibling(2, zone->sibling(1));
                ReplaceRegionIfNeeded(zone, se, 2, 1, 1);

                zone->set_sibling(1, zone->sibling(0));
                ReplaceRegionIfNeeded(zone, se, 1, 1, 0);

                zone->set_sibling(0, se);
                ReplaceRegionIfNeeded(zone, se, 0, 0, 0);

                zone->UpdateViewportCoord();
                DCHECK_EQ(com::ZoneComponent::kNE, zone->want());
            } else if (zone->region()->AtSouthEast(viewport_center.x, viewport_center.y)) {
                //    [SE->0] | [0->1]
                // -----------+---------
                //        [2] | [1->NW]
                com::RegionComponent *se = zone->region();
                zone->set_region(zone->sibling(1));
                ReplaceRegionIfNeeded(zone, se, -1, 1, 1);

                zone->set_sibling(1, zone->sibling(0));
                ReplaceRegionIfNeeded(zone, se, 1, 1, 0);

                zone->set_sibling(0, se);
                ReplaceRegionIfNeeded(zone, se, 0, 0, 0);

                ReplaceRegionIfNeeded(zone, se, 2, 0, 1);

                zone->UpdateViewportCoord();
                DCHECK_EQ(com::ZoneComponent::kNW, zone->want());
            } else {
                com::RegionComponent *se = zone->region();
                ReplaceRegionIfNeeded(zone, se, 0, 1, 0);
                ReplaceRegionIfNeeded(zone, se, 1, 1, 1);
                ReplaceRegionIfNeeded(zone, se, 2, 0, 1);
            }
            DCHECK(zone->region()->AtBound(viewport_center.x, viewport_center.y));
        } break;
        //
        //    [0] | SW
        // -------+------
        //    [2] | [1]
        case com::ZoneComponent::kSW: {
            if (zone->region()->AtSouth(viewport_center.x, viewport_center.y)) {
                //    [0] | [SW->1]
                // -------+--------
                //    [2] | [1->NW]
                com::RegionComponent *sw = zone->region();
                zone->set_region(zone->sibling(1));
                ReplaceRegionIfNeeded(zone, sw, 2, 0, 1);

                zone->set_sibling(1, sw);
                ReplaceRegionIfNeeded(zone, sw, 1, 0, 0);

                ReplaceRegionIfNeeded(zone, sw, 0, -1, 0);

                ReplaceRegionIfNeeded(zone, sw, 2, -1, 1);

                zone->UpdateViewportCoord();
                DCHECK_EQ(com::ZoneComponent::kNW, zone->want());
            } else if (zone->region()->AtWest(viewport_center.x, viewport_center.y)) {
                //    [0->SE] | [SW->0]
                // -----------+------
                //        [2] | [1]
                com::RegionComponent *sw = zone->region();
                zone->set_region(zone->sibling(0));
                ReplaceRegionIfNeeded(zone, sw, -1, -1, 0);

                zone->set_sibling(0, sw);
                ReplaceRegionIfNeeded(zone, sw, 0, 0, 0);

                ReplaceRegionIfNeeded(zone, sw, 1, 0, 1);

                ReplaceRegionIfNeeded(zone, sw, 2, -1, 1);

                zone->UpdateViewportCoord();
                DCHECK_EQ(com::ZoneComponent::kSE, zone->want());
            } else if (zone->region()->AtSouthWest(viewport_center.x, viewport_center.y)) {
                //        [0] | [SW->1]
                // -----------+--------
                //    [2->NE] | [1->2]
                com::RegionComponent *sw = zone->region();
                zone->set_region(zone->sibling(2));
                ReplaceRegionIfNeeded(zone, sw, -1, -1, 1);

                zone->set_sibling(2, zone->sibling(1));
                ReplaceRegionIfNeeded(zone, sw, 2, 0, 1);

                zone->set_sibling(1, sw);
                ReplaceRegionIfNeeded(zone, sw, 1, 0, 0);

                ReplaceRegionIfNeeded(zone, sw, 0, -1, 0);

                zone->UpdateViewportCoord();
                DCHECK_EQ(com::ZoneComponent::kNE, zone->want());
            } else {
                com::RegionComponent *sw = zone->region();
                ReplaceRegionIfNeeded(zone, sw, 0, -1, 0);
                ReplaceRegionIfNeeded(zone, sw, 1, 0, 1);
                ReplaceRegionIfNeeded(zone, sw, 2, -1, 1);
            }
            DCHECK(zone->region()->AtBound(viewport_center.x, viewport_center.y));
        } break;
        //
        //   [0] | [1]
        // ------+------
        //  [NE] | [2]
        case com::ZoneComponent::kNE: {
            if (zone->region()->AtEast(viewport_center.x, viewport_center.y)) {
                //        [0] | [1]
                // -----------+---------
                //    [NE->2] | [2->NW]
                com::RegionComponent *ne = zone->region();
                zone->set_region(zone->sibling(2));
                ReplaceRegionIfNeeded(zone, ne, -1, 1, 0);

                zone->set_sibling(2, ne);
                ReplaceRegionIfNeeded(zone, ne, 2, 0, 0);

                ReplaceRegionIfNeeded(zone, ne, 0, 0, -1);

                ReplaceRegionIfNeeded(zone, ne, 1, 1, -1);

                zone->UpdateViewportCoord();
                DCHECK_EQ(com::ZoneComponent::kNW, zone->want());
            } else if (zone->region()->AtNorth(viewport_center.x, viewport_center.y)) {
                //     [0->SE] | [1->0]
                // ------------+--------
                //     [NE->2] | [2->1]
                com::RegionComponent *ne = zone->region();
                zone->set_region(zone->sibling(0));
                ReplaceRegionIfNeeded(zone, ne, -1, 0, -1);

                zone->set_sibling(0, zone->sibling(1));
                ReplaceRegionIfNeeded(zone, ne, 0, 1, -1);

                zone->set_sibling(1, zone->sibling(2));
                ReplaceRegionIfNeeded(zone, ne, 1, 1, 0);

                zone->set_sibling(2, ne);
                ReplaceRegionIfNeeded(zone, ne, 2, 0, 0);

                zone->UpdateViewportCoord();
                DCHECK_EQ(com::ZoneComponent::kSE, zone->want());
            } else if (zone->region()->AtNorthEast(viewport_center.x, viewport_center.y)) {
                //        [0] | [1->SW]
                // -----------+---------
                //    [NE->2] | [2->1]
                com::RegionComponent *ne = zone->region();
                zone->set_region(zone->sibling(1));
                ReplaceRegionIfNeeded(zone, ne, -1, 1, -1);

                zone->set_sibling(1, zone->sibling(2));
                ReplaceRegionIfNeeded(zone, ne, 1, 1, 0);

                zone->set_sibling(2, ne);
                ReplaceRegionIfNeeded(zone, ne, 2, 0, 0);

                zone->UpdateViewportCoord();
                DCHECK_EQ(com::ZoneComponent::kSW, zone->want());
            } else {
                com::RegionComponent *ne = zone->region();
                ReplaceRegionIfNeeded(zone, ne, 0, 0, -1);
                ReplaceRegionIfNeeded(zone, ne, 1, -1, -1);
                ReplaceRegionIfNeeded(zone, ne, 2, 1, 0);
            }
            DCHECK(zone->region()->AtBound(viewport_center.x, viewport_center.y));
        } break;
        //
        //   [0] | [1]
        // ------+------
        //   [2] | [NW]
        case com::ZoneComponent::kNW: {
            if (zone->region()->AtWest(viewport_center.x, viewport_center.y)) {
                //       [0] | [1]
                // ----------+------
                //   [2->NE] | [NW->2]
                com::RegionComponent *nw = zone->region();
                zone->set_region(zone->sibling(2));
                ReplaceRegionIfNeeded(zone, nw, -1, -1, 0);

                zone->set_sibling(2, nw);
                ReplaceRegionIfNeeded(zone, nw, 2, 0, 0);

                ReplaceRegionIfNeeded(zone, nw, 1, 0, -1);

                ReplaceRegionIfNeeded(zone, nw, 0, -1, -1);

                zone->UpdateViewportCoord();
                DCHECK_EQ(com::ZoneComponent::kNE, zone->want());
            } else if (zone->region()->AtNorth(viewport_center.x, viewport_center.x)) {
                //   [0] | [1->SW]
                // ------+---------
                //   [2] | [NW->1]
                com::RegionComponent *nw = zone->region();
                zone->set_region(zone->sibling(1));
                ReplaceRegionIfNeeded(zone, nw, -1, 0, -1);

                zone->set_sibling(1, nw);
                ReplaceRegionIfNeeded(zone, nw, 1, 0, 0);

                ReplaceRegionIfNeeded(zone, nw, 2, -1, 0);

                ReplaceRegionIfNeeded(zone, nw, 0, -1, -1);

                zone->UpdateViewportCoord();
                DCHECK_EQ(com::ZoneComponent::kSW, zone->want());
            } else if (zone->region()->AtNorthWest(viewport_center.x, viewport_center.y)) {
                //   [0->SE] | [1->0]
                // ----------+---------
                //       [2] | [NW->1]
                com::RegionComponent *nw = zone->region();
                zone->set_region(zone->sibling(0));
                ReplaceRegionIfNeeded(zone, nw, -1, -1, -1);

                zone->set_sibling(0, zone->sibling(1));
                ReplaceRegionIfNeeded(zone, nw, 0, 0, -1);

                zone->set_sibling(1, nw);
                ReplaceRegionIfNeeded(zone, nw, 1, 0, 0);

                ReplaceRegionIfNeeded(zone, nw, 2, -1, 0);

                zone->UpdateViewportCoord();
                DCHECK_EQ(com::ZoneComponent::kSE, zone->want());
            } else {
                com::RegionComponent *nw = zone->region();
                ReplaceRegionIfNeeded(zone, nw, 0, -1, -1);
                ReplaceRegionIfNeeded(zone, nw, 1, 0, -1);
                ReplaceRegionIfNeeded(zone, nw, 2, -1, 0);
            }
            DCHECK(zone->region()->AtBound(viewport_center.x, viewport_center.y));
        } break;
        case com::ZoneComponent::kNone: break;
        default: NOREACHED(); break;
    }
}

void ZoneLoadingSystem::ReplaceRegionIfNeeded(com::ZoneComponent *zone, com::RegionComponent *region, int index, int dx,
                                              int dy) {
    int x = region->global_coord().x + dx * kRegionSize;
    int y = region->global_coord().y + dy * kRegionSize;
    if (index < 0) {
        if (!zone->region() || !zone->region()->GlobalCoordEqual(x, y)) {
            delete zone->region();
            DLOG(INFO) << "Load region (" << x << ", " << y << ") set region.";
            zone->set_region(LoadRegion(x, y));
        }
    } else {
        if (!zone->sibling(index) || !zone->sibling(index)->GlobalCoordEqual(x, y)) {
            delete zone->sibling(index);
            DLOG(INFO) << "Load region (" << x << ", " << y << ") set sibling[" << index << "].";
            zone->set_sibling(index, LoadRegion(x, y));
        }
    }
}

com::RegionComponent *ZoneLoadingSystem::LoadRegion(int x, int y) {
    com::RegionComponent *region = new com::RegionComponent();
    region->set_global_coord({x, y});
    Game::This()->random_zone()->Update(region);
    return region;
}

}  // namespace sys

}  // namespace nyaa
