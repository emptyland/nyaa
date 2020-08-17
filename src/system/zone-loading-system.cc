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

    com::ZoneComponent::Want want = zone->UpdateViewportCoord();
    switch (want) {
        case com::ZoneComponent::kE: {
            if (zone->region()->AtEast(viewport_center.x, viewport_center.y)) {
                int x = zone->region()->global_coord().x + kRegionSize;
                int y = zone->region()->global_coord().y;
                if (!zone->sibling(0) || !zone->sibling(0)->GlobalCoordEqual(x, y)) {
                    delete zone->sibling(0);
                    zone->set_sibling(0, zone->region());
                    zone->set_region(LoadRegion(x, y));
                }
                zone->UpdateViewportCoord();
            }
            DCHECK(zone->region()->AtBound(viewport_center.x, viewport_center.y));
        } break;
        case com::ZoneComponent::kS: {
            if (zone->region()->AtSouth(viewport_center.x, viewport_center.y)) {
                int x = zone->region()->global_coord().x;
                int y = zone->region()->global_coord().y + kRegionSize;
                if (!zone->sibling(0) || !zone->sibling(0)->GlobalCoordEqual(x, y)) {
                    delete zone->sibling(0);
                    zone->set_sibling(0, zone->region());
                    zone->set_region(LoadRegion(x, y));
                }
                zone->UpdateViewportCoord();
            }
            DCHECK(zone->region()->AtBound(viewport_center.x, viewport_center.y));
        } break;
        case com::ZoneComponent::kW: {
            if (zone->region()->AtWest(viewport_center.x, viewport_center.y)) {
                int x = zone->region()->global_coord().x - kRegionSize;
                int y = zone->region()->global_coord().y;
                if (!zone->sibling(0) || !zone->sibling(0)->GlobalCoordEqual(x, y)) {
                    delete zone->sibling(0);
                    zone->set_sibling(0, zone->region());
                    zone->set_region(LoadRegion(x, y));
                }
                zone->UpdateViewportCoord();
            }
            DCHECK(zone->region()->AtBound(viewport_center.x, viewport_center.y));
        } break;
        case com::ZoneComponent::kN: {
            if (zone->region()->AtNorth(viewport_center.x, viewport_center.y)) {
                int x = zone->region()->global_coord().x;
                int y = zone->region()->global_coord().y - kRegionSize;
                if (!zone->sibling(0) || !zone->sibling(0)->GlobalCoordEqual(x, y)) {
                    delete zone->sibling(0);
                    zone->set_sibling(0, zone->region());
                    zone->set_region(LoadRegion(x, y));
                }
                zone->UpdateViewportCoord();
            }
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
                int x = zone->region()->global_coord().x + kRegionSize;
                int y = zone->region()->global_coord().y;

                com::RegionComponent *se = zone->region();
                zone->set_region(zone->sibling(0));
                if (!zone->region() || !zone->region()->GlobalCoordEqual(x, y)) { zone->set_region(LoadRegion(x, y)); }
                zone->set_sibling(0, se);

                x = zone->region()->global_coord().x + kRegionSize;
                y = zone->region()->global_coord().y + kRegionSize;
                if (!zone->sibling(1) || !zone->sibling(1)->GlobalCoordEqual(x, y)) {
                    delete zone->sibling(1);
                    zone->set_sibling(1, LoadRegion(x, y));
                }

                x = zone->region()->global_coord().x;
                y = zone->region()->global_coord().y + kRegionSize;
                if (!zone->sibling(2) || !zone->sibling(2)->GlobalCoordEqual(x, y)) {
                    delete zone->sibling(2);
                    zone->set_sibling(2, LoadRegion(x, y));
                }

                zone->UpdateViewportCoord();
            } else if (zone->region()->AtSouth(viewport_center.x, viewport_center.y)) {
                //    [SE->0]  | [0->1]
                // ------------+------
                //    [2->NE]  | [1->2]
                int x = zone->region()->global_coord().x;
                int y = zone->region()->global_coord().y + kRegionSize;

                com::RegionComponent *se = zone->region();
                zone->set_region(zone->sibling(2));
                if (!zone->region() || !zone->region()->GlobalCoordEqual(x, y)) {
                    delete zone->region();
                    zone->set_region(LoadRegion(x, y));
                }

                zone->set_sibling(2, zone->sibling(1));
                x = zone->region()->global_coord().x + kRegionSize;
                y = zone->region()->global_coord().y + kRegionSize;
                if (!zone->sibling(2) || !zone->sibling(2)->GlobalCoordEqual(x, y)) {
                    delete zone->sibling(2);
                    zone->set_sibling(2, LoadRegion(x, y));
                }

                zone->set_sibling(1, zone->sibling(0));
                x = zone->region()->global_coord().x + kRegionSize;
                y = zone->region()->global_coord().y;
                if (!zone->sibling(1) || !zone->sibling(1)->GlobalCoordEqual(x, y)) {
                    delete zone->sibling(1);
                    zone->set_sibling(1, LoadRegion(x, y));
                }
                zone->set_sibling(0, DCHECK_NOTNULL(se));

                zone->UpdateViewportCoord();
            } else if (zone->region()->AtSouthEast(viewport_center.x, viewport_center.y)) {
                //    [SE->0] | [0->1]
                // -----------+---------
                //        [2] | [1->NW]
                int x = zone->region()->global_coord().x + kRegionSize;
                int y = zone->region()->global_coord().y + kRegionSize;

                com::RegionComponent *se = zone->region();
                zone->set_region(zone->sibling(1));
                if (!zone->region() || !zone->region()->GlobalCoordEqual(x, y)) {
                    delete zone->region();
                    zone->set_region(LoadRegion(x, y));
                }

                zone->set_sibling(1, zone->sibling(0));
                x = zone->region()->global_coord().x + kRegionSize;
                y = zone->region()->global_coord().y;
                if (!zone->sibling(1) || !zone->sibling(1)->GlobalCoordEqual(x, y)) {
                    delete zone->sibling(1);
                    zone->set_sibling(1, LoadRegion(x, y));
                }

                zone->set_sibling(0, DCHECK_NOTNULL(se));

                x = zone->region()->global_coord().x;
                y = zone->region()->global_coord().y + kRegionSize;
                if (!zone->sibling(2) || !zone->sibling(2)->GlobalCoordEqual(x, y)) {
                    delete zone->sibling(2);
                    zone->set_sibling(2, LoadRegion(x, y));
                }

                zone->UpdateViewportCoord();
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
                int x = zone->region()->global_coord().x;
                int y = zone->region()->global_coord().y + kRegionSize;

                com::RegionComponent *sw = zone->region();
                zone->set_region(zone->sibling(1));
                if (!zone->region() || !zone->region()->GlobalCoordEqual(x, y)) {
                    delete zone->region();
                    zone->set_region(LoadRegion(x, y));
                }
                zone->set_sibling(1, sw);

                x = zone->region()->global_coord().x - kRegionSize;
                y = zone->region()->global_coord().y;
                if (!zone->sibling(0) || !zone->sibling(0)->GlobalCoordEqual(x, y)) {
                    delete zone->sibling(0);
                    zone->set_sibling(0, LoadRegion(x, y));
                }

                x = zone->region()->global_coord().x - kRegionSize;
                y = zone->region()->global_coord().y + kRegionSize;
                if (!zone->sibling(2) || !zone->sibling(2)->GlobalCoordEqual(x, y)) {
                    delete zone->sibling(2);
                    zone->set_sibling(2, LoadRegion(x, y));
                }

                zone->UpdateViewportCoord();

            } else if (zone->region()->AtWest(viewport_center.x, viewport_center.y)) {
                //    [0->SE] | [SW->0]
                // -----------+------
                //        [2] | [1]
                int x = zone->region()->global_coord().x - kRegionSize;
                int y = zone->region()->global_coord().y;

                com::RegionComponent *sw = zone->region();
                zone->set_region(zone->sibling(0));
                if (!zone->region() || !zone->region()->GlobalCoordEqual(x, y)) {
                    delete zone->region();
                    zone->set_region(LoadRegion(x, y));
                }
                zone->set_sibling(0, sw);

                x = zone->region()->global_coord().x;
                y = zone->region()->global_coord().y + kRegionSize;
                if (!zone->sibling(1) || !zone->sibling(1)->GlobalCoordEqual(x, y)) {
                    delete zone->sibling(1);
                    zone->set_sibling(1, LoadRegion(x, y));
                }

                x = zone->region()->global_coord().x - kRegionSize;
                y = zone->region()->global_coord().y + kRegionSize;
                if (!zone->sibling(2) || !zone->sibling(2)->GlobalCoordEqual(x, y)) {
                    delete zone->sibling(2);
                    zone->set_sibling(2, LoadRegion(x, y));
                }

                zone->UpdateViewportCoord();

            } else if (zone->region()->AtSouthWest(viewport_center.x, viewport_center.y)) {
                //        [0] | [SW->1]
                // -----------+--------
                //    [2->NE] | [1->2]
                int x = zone->region()->global_coord().x - kRegionSize;
                int y = zone->region()->global_coord().y + kRegionSize;

                com::RegionComponent *sw = zone->region();
                zone->set_region(zone->sibling(2));
                if (!zone->region() || !zone->region()->GlobalCoordEqual(x, y)) {
                    delete zone->region();
                    zone->set_region(LoadRegion(x, y));
                }

                zone->set_sibling(2, zone->sibling(1));
                x = zone->region()->global_coord().x - kRegionSize;
                y = zone->region()->global_coord().y + kRegionSize;
                if (!zone->sibling(2) || !zone->sibling(2)->GlobalCoordEqual(x, y)) {
                    delete zone->sibling(2);
                    zone->set_sibling(2, LoadRegion(x, y));
                }

                zone->set_sibling(1, sw);
                x = zone->region()->global_coord().x;
                y = zone->region()->global_coord().y + kRegionSize;
                if (!zone->sibling(1) || !zone->sibling(1)->GlobalCoordEqual(x, y)) {
                    delete zone->sibling(1);
                    zone->set_sibling(1, LoadRegion(x, y));
                }

                zone->UpdateViewportCoord();
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
            } else if (zone->region()->AtNorth(viewport_center.x, viewport_center.y)) {
                //     [0->SE] | [1->0]
                // ------------+--------
                //     [NE->2] | [2->1]
            } else if (zone->region()->AtNorthEast(viewport_center.x, viewport_center.y)) {
                //        [0] | [1->SW]
                // -----------+---------
                //    [NE->2] | [2->1]
            }
            DCHECK(zone->region()->AtBound(viewport_center.x, viewport_center.y));
        } break;
        //
        //   [0] | [1]
        // ------+------
        //   [2] | [NW]
        case com::ZoneComponent::kNW: {
            if (zone->region()->AtWest(viewport_center.x, viewport_center.y)) {
                //   [0] | [1]
                // ------+------
                //   [2] | [NW]
            } else if (zone->region()->AtNorth(viewport_center.x, viewport_center.x)) {
                //   [0] | [1]
                // ------+------
                //   [2] | [NW]
            } else if (zone->region()->AtNorthWest(viewport_center.x, viewport_center.y)) {
                //   [0] | [1]
                // ------+------
                //   [2] | [NW]
            }
            DCHECK(zone->region()->AtBound(viewport_center.x, viewport_center.y));
        } break;
        case com::ZoneComponent::kNone: break;
        default: break;
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
