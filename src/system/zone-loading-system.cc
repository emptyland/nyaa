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
            if (viewport_center.x >= zone->region()->global_coord().x + kRegionSize) {
                int x = zone->region()->global_coord().x + kRegionSize;
                int y = zone->region()->global_coord().y;
                if (!zone->sibling(0)) {
                    zone->set_sibling(0, zone->region());
                    zone->set_region(LoadRegion(x, y));
                } else if (x != zone->sibling(0)->global_coord().x && y != zone->sibling(0)->global_coord().y) {
                    delete zone->sibling(0);
                    zone->set_sibling(0, zone->region());
                    zone->set_region(LoadRegion(x, y));
                }
                zone->UpdateViewportCoord();
            }
        } break;
        case com::ZoneComponent::kS: {
            if (viewport_center.y >= zone->region()->global_coord().y + kRegionSize) {
                int x = zone->region()->global_coord().x;
                int y = zone->region()->global_coord().y + kRegionSize;
                if (!zone->sibling(0)) {
                    zone->set_sibling(0, zone->region());
                    zone->set_region(LoadRegion(x, y));
                } else if (x != zone->sibling(0)->global_coord().x && y != zone->sibling(0)->global_coord().y) {
                    delete zone->sibling(0);
                    zone->set_sibling(0, zone->region());
                    zone->set_region(LoadRegion(x, y));
                }
                zone->UpdateViewportCoord();
            }
        } break;
        case com::ZoneComponent::kW: {
            if (viewport_center.x < zone->region()->global_coord().x) {
                int x = zone->region()->global_coord().x - kRegionSize;
                int y = zone->region()->global_coord().y;
                if (!zone->sibling(0)) {
                    zone->set_sibling(0, zone->region());
                    zone->set_region(LoadRegion(x, y));
                } else if (x != zone->sibling(0)->global_coord().x && y != zone->sibling(0)->global_coord().y) {
                    delete zone->sibling(0);
                    zone->set_sibling(0, zone->region());
                    zone->set_region(LoadRegion(x, y));
                }
                zone->UpdateViewportCoord();
            }
        } break;
        case com::ZoneComponent::kN: {
            if (viewport_center.y < zone->region()->global_coord().y) {
                int x = zone->region()->global_coord().x;
                int y = zone->region()->global_coord().y - kRegionSize;
                if (!zone->sibling(0)) {
                    zone->set_sibling(0, zone->region());
                    zone->set_region(LoadRegion(x, y));
                } else if (x != zone->sibling(0)->global_coord().x && y != zone->sibling(0)->global_coord().y) {
                    delete zone->sibling(0);
                    zone->set_sibling(0, zone->region());
                    zone->set_region(LoadRegion(x, y));
                }
                zone->UpdateViewportCoord();
            }
        } break;
        //
        //     SE | [0]
        // -------+------
        //    [2] | [1]
        case com::ZoneComponent::kSE: {
            if (viewport_center.x >= zone->region()->global_coord().x + kRegionSize &&
                viewport_center.y < zone->region()->global_coord().y + kRegionSize) {
                //    [SE->0] | [0->SW]
                // -----------+---------
                //        [2] | [1]
                int x = zone->region()->global_coord().x + kRegionSize;
                int y = zone->region()->global_coord().y;

                com::RegionComponent *se = zone->region();
                zone->set_region(zone->sibling(0));
                if (!zone->region()) { zone->set_region(LoadRegion(x, y)); }
                zone->set_sibling(0, se);
                if (!zone->sibling(1)) {
                    x = zone->region()->global_coord().x + kRegionSize;
                    y = zone->region()->global_coord().y + kRegionSize;
                    zone->set_sibling(1, LoadRegion(x, y));
                }
                if (!zone->sibling(2)) {
                    x = zone->region()->global_coord().x;
                    y = zone->region()->global_coord().y + kRegionSize;
                    zone->set_sibling(2, LoadRegion(x, y));
                }

                zone->UpdateViewportCoord();
            } else if (viewport_center.x < zone->region()->global_coord().x + kRegionSize &&
                       viewport_center.y >= zone->region()->global_coord().y + kRegionSize) {
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
            } else if (viewport_center.x >= zone->region()->global_coord().x + kRegionSize &&
                       viewport_center.y >= zone->region()->global_coord().y + kRegionSize) {
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
            }
        } break;
        //
        //    [0] | SW
        // -------+------
        //    [2] | [1]
        case com::ZoneComponent::kSW: {

        } break;
        case com::ZoneComponent::kNE: break;
        case com::ZoneComponent::kNW: break;
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
