#include "component/zone-component.h"

namespace nyaa {

namespace com {

ZoneComponent::ZoneComponent() {
    ClearRegions();
}

ZoneComponent::~ZoneComponent() {
    ClearRegions();
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
    const int lx = x % kRegionSize;
    const int ly = y % kRegionSize;

    if (center()->AtEast(x, y)) {
        return &region(2, 1)->floor(z)->cubes[lx][ly];
    } else if (center()->AtWest(x, y)) {
        return &region(0, 1)->floor(z)->cubes[lx][ly];
    } else if (center()->AtSouth(x, y)) {
        return &region(1, 2)->floor(z)->cubes[lx][ly];
    } else if (center()->AtNorth(x, y)) {
        return &region(1, 0)->floor(z)->cubes[lx][ly];
    } else if (center()->AtSouthEast(x, y)) {
        return &region(2, 2)->floor(z)->cubes[lx][ly];
    } else if (center()->AtSouthWest(x, y)) {
        return &region(0, 2)->floor(z)->cubes[lx][ly];
    } else if (center()->AtNorthEast(x, y)) {
        return &region(2, 0)->floor(z)->cubes[lx][ly];
    } else if (center()->AtNorthWest(x, y)) {
        return &region(0, 0)->floor(z)->cubes[lx][ly];
    }
    DCHECK(center()->AtBound(x, y));
    return &center()->floor(z)->cubes[lx][ly];
}

const char *ZoneComponent::kDirectionText[] = {
    "None", "East", "South", "West", "North", "SouthEast", "NorthEast", "SouthWest", "NorthWest",
};

ZoneComponent::Direction ZoneComponent::WantSibling() {
    Boundi region{center()->global_coord().x, center()->global_coord().y, center()->global_coord().x + kRegionSize,
                  center()->global_coord().y + kRegionSize};
    Boundi view{viewport().round_down_center_x() - viewport().bound().x / 2,
                viewport().round_down_center_y() - viewport().bound().y / 2,
                viewport().round_down_center_x() + viewport().bound().x / 2,
                viewport().round_down_center_y() + viewport().bound().y / 2};

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
