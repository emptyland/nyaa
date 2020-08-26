#include "component/zone-component.h"

namespace nyaa {

namespace com {

ZoneComponent::ZoneComponent() { ::memset(sibling_, 0, sizeof(sibling_)); }

ZoneComponent::~ZoneComponent() {
    delete region_;
    for (int i = 0; i < arraysize(sibling_); i++) { delete sibling(i); }
}

CubeComponent *ZoneComponent::CubeAt(int i, int j, int z) {
    DCHECK_GE(i, 0);
    DCHECK_LT(i, viewport_.bound().x);
    DCHECK_GE(j, 0);
    DCHECK_LT(j, viewport_.bound().y);
    DCHECK_GE(z, 0);
    DCHECK_LT(z, kTerrainMaxLevels);

    const int dx = viewport_.bound().x / 2;
    const int dy = viewport_.bound().y / 2;

    Vector2i dest{viewport_.round_down_center_x() - dx, viewport_.round_down_center_y() - dy};
    DCHECK_GE(dest.x, 0);
    DCHECK_GE(dest.y, 0);

    dest.x += i;
    dest.y += j;

    return Cube(dest.x, dest.y, z);
}

CubeComponent *ZoneComponent::Cube(int x, int y, int z) {

    const int limit_x = region()->global_coord().x + kRegionSize;
    const int limit_y = region()->global_coord().y + kRegionSize;
    const int lx      = x % kRegionSize;
    const int ly      = y % kRegionSize;
    switch (want()) {
        case kNone: return &region()->floor(z)->cubes[lx][ly];
        case kE:
            if (region()->AtEast(x, y)) {
                return &sibling(0)->floor(z)->cubes[lx][ly];
            } else {
                DCHECK(region()->AtBound(x, y));
                return &region()->floor(z)->cubes[lx][ly];
            }
            break;
        case kS:
            if (region()->AtSouth(x, y)) {
                return &sibling(0)->floor(z)->cubes[lx][ly];
            } else {
                DCHECK(region()->AtBound(x, y));
                return &region()->floor(z)->cubes[lx][ly];
            }
            break;
        case kW:
            if (region()->AtWest(x, y)) {
                return &sibling(0)->floor(z)->cubes[lx][ly];
            } else {
                DCHECK(region()->AtBound(x, y));
                return &region()->floor(z)->cubes[lx][ly];
            }
            break;
        case kN:
            if (region()->AtNorth(x, y)) {
                return &sibling(0)->floor(z)->cubes[lx][ly];
            } else {
                DCHECK(region()->AtBound(x, y));
                return &region()->floor(z)->cubes[lx][ly];
            }
            break;

        //
        //   SE | [0]
        // -----+-----
        //  [2] | [1]
        case kSE:
            if (region()->AtBound(x, y)) {
                return &region()->floor(z)->cubes[lx][ly];
            } else if (region()->AtEast(x, y)) {
                return &sibling(0)->floor(z)->cubes[lx][ly];
            } else if (region()->AtSouthEast(x, y)) {
                return &sibling(1)->floor(z)->cubes[lx][ly];
            } else {
                DCHECK(region()->AtSouth(x, y));
                return &sibling(2)->floor(z)->cubes[lx][ly];
            }
            break;

        //
        //   [0] | [1]
        // ------+-----
        //    NE | [2]
        case kNE:
            if (region()->AtNorth(x, y)) {
                return &sibling(0)->floor(z)->cubes[lx][ly];
            } else if (region()->AtNorthEast(x, y)) {
                return &sibling(1)->floor(z)->cubes[lx][ly];
            } else if (region()->AtEast(x, y)) {
                return &sibling(2)->floor(z)->cubes[lx][ly];
            } else {
                DCHECK(region()->AtBound(x, y));
                return &region()->floor(z)->cubes[lx][ly];
            }
            break;

        //
        //  [0] | SW
        // -----+-----
        //  [2] | [1]
        case kSW:
            if (region()->AtWest(x, y)) {
                return &sibling(0)->floor(z)->cubes[lx][ly];
            } else if (region()->AtBound(x, y)) {
                return &region()->floor(z)->cubes[lx][ly];
            } else if (region()->AtSouth(x, y)) {
                return &sibling(1)->floor(z)->cubes[lx][ly];
            } else {
                DCHECK(region()->AtSouthWest(x, y));
                return &sibling(2)->floor(z)->cubes[lx][ly];
            }
            break;

        //
        //  [0] | [1]
        // -----+-----
        //  [2] | NW
        case kNW:
            if (region()->AtNorthWest(x, y)) {
                return &sibling(0)->floor(z)->cubes[lx][ly];
            } else if (region()->AtNorth(x, y)) {
                return &sibling(1)->floor(z)->cubes[lx][ly];
            } else if (region()->AtBound(x, y)) {
                return &region()->floor(z)->cubes[lx][ly];
            } else {
                DCHECK(region()->AtWest(x, y));
                return &sibling(2)->floor(z)->cubes[lx][ly];
            }
            break;
        default: NOREACHED(); break;
    }
    return nullptr;
}

const char *ZoneComponent::kDirectionText[] = {
    "None", "East", "South", "West", "North", "SouthEast", "NorthEast", "SouthWest", "NorthWest",
};

ZoneComponent::Direction ZoneComponent::WantSibling() {
    Boundi region{region_->global_coord().x, region_->global_coord().y, region_->global_coord().x + kRegionSize,
                  region_->global_coord().y + kRegionSize};
    Boundi view{viewport_.round_down_center_x() - viewport_.bound().x / 2,
                viewport_.round_down_center_y() - viewport_.bound().y / 2,
                viewport_.round_down_center_x() + viewport_.bound().x / 2,
                viewport_.round_down_center_y() + viewport_.bound().y / 2};

    Direction want = kNone;
    if (view.w > region.w) {
        want = kE;
    } else if (view.x < region.x) {
        want = kW;
    }

    if (view.h > region.h) {
        switch (want) {
            case kNone: want = kS; break;
            case kE: want = kSE; break;
            case kW: want = kSW; break;
            default: NOREACHED(); break;
        }
    }
    if (view.y < region.y) {
        switch (want) {
            case kNone: want = kN; break;
            case kE: want = kNE; break;
            case kW: want = kNW; break;
            default: NOREACHED(); break;
        }
    }
    return want;
}

}  // namespace com

}  // namespace nyaa
