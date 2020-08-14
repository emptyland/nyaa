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

    Vertex2i dest{viewport_.center_coord().x - dx, viewport_.center_coord().y - dy};
    DCHECK_GE(dest.x, 0);
    DCHECK_GE(dest.y, 0);

    dest.x += i;
    dest.y += j;

    const int limit_x = region_->global_coord().x + kRegionSize;
    const int limit_y = region_->global_coord().y + kRegionSize;
    const int lx      = dest.x % kRegionSize;
    const int ly      = dest.y % kRegionSize;
    switch (want_) {
        case kNone: return &region_->floor(z)->cubes[lx][ly];
        case kE:
            if (dest.x < limit_x) {
                return &region_->floor(z)->cubes[lx][ly];
            } else {
                return &sibling(0)->floor(z)->cubes[lx][ly];
            }
            break;
        case kS:
            if (dest.y < limit_y) {
                return &region_->floor(z)->cubes[lx][ly];
            } else {
                return &sibling(0)->floor(z)->cubes[lx][ly];
            }
            break;
        case kW:
            if (dest.x >= region_->global_coord().x) {
                return &region_->floor(z)->cubes[lx][ly];
            } else {
                return &sibling(0)->floor(z)->cubes[lx][ly];
            }
            break;
        case kN:
            if (dest.y >= region_->global_coord().y) {
                return &region_->floor(z)->cubes[lx][ly];
            } else {
                return &sibling(0)->floor(z)->cubes[lx][ly];
            }
            break;
        case kSE:
            if (dest.x < limit_x && dest.y < limit_y) {
                return &region_->floor(z)->cubes[lx][ly];
            } else if (dest.x >= limit_x && dest.y < limit_y) {
                return &sibling(0)->floor(z)->cubes[lx][ly];
            } else if (dest.x >= limit_x && dest.y >= limit_y) {
                return &sibling(1)->floor(z)->cubes[lx][ly];
            } else {
                return &sibling(2)->floor(z)->cubes[lx][ly];
            }
            break;
        case kNE:
            if (dest.x < limit_x && dest.y >= region_->global_coord().y) {
                return &sibling(0)->floor(z)->cubes[lx][ly];
            } else if (dest.x < limit_x && dest.y < region_->global_coord().y) {
                return &sibling(1)->floor(z)->cubes[lx][ly];
            } else if (dest.x >= limit_x && dest.y < region_->global_coord().y) {
                return &sibling(2)->floor(z)->cubes[lx][ly];
            } else {
                return &region_->floor(z)->cubes[lx][ly];
            }
            break;
        case kSW:
            if (dest.x < region_->global_coord().x && dest.y < limit_y) {
                return &sibling(0)->floor(z)->cubes[lx][ly];
            } else if (dest.x >= region_->global_coord().x && dest.y < limit_y) {
                return &region_->floor(z)->cubes[lx][ly];
            } else if (dest.x >= region_->global_coord().x && dest.y >= limit_y) {
                return &sibling(1)->floor(z)->cubes[lx][ly];
            } else {
                return &sibling(2)->floor(z)->cubes[lx][ly];
            }
            break;
        case kNW:
            if (dest.x < region_->global_coord().x && dest.y < region_->global_coord().y) {
                return &sibling(0)->floor(z)->cubes[lx][ly];
            } else if (dest.x >= region_->global_coord().x && dest.y < region_->global_coord().y) {
                return &sibling(1)->floor(z)->cubes[lx][ly];
            } else if (dest.x >= region_->global_coord().x && dest.y >= region_->global_coord().y) {
                return &region_->floor(z)->cubes[lx][ly];
            } else {
                return &sibling(2)->floor(z)->cubes[lx][ly];
            }
            break;
        default: NOREACHED(); break;
    }
    return nullptr;
}

ZoneComponent::Want ZoneComponent::WantSibling() {
    Vertex4i region{region_->global_coord().x, region_->global_coord().y, region_->global_coord().x + kRegionSize,
                    region_->global_coord().y + kRegionSize};
    Vertex4i view{
        viewport_.center_coord().x - viewport_.bound().x / 2, viewport_.center_coord().y - viewport_.bound().y / 2,
        viewport_.center_coord().x + viewport_.bound().x / 2, viewport_.center_coord().y + viewport_.bound().y / 2};

    Want want = kNone;
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
