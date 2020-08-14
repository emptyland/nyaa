#pragma once
#ifndef NYAA_COMPONENT_REGION_COMPONENT_H_
#define NYAA_COMPONENT_REGION_COMPONENT_H_

#include "component/cube-component.h"
#include "game/constants.h"

namespace nyaa {

namespace com {

class RegionComponent {
public:
    using Floor = com::FloorComponent<kRegionSize, kRegionSize>;

    DEF_VAL_PROP_RW(Vertex2i, global_coord);

    Floor *floor(int i) {
        DCHECK_GE(i, 0);
        DCHECK_LT(i, kTerrainMaxLevels);
        return &terrain_[i];
    }

private:
    Vertex2i global_coord_ = {0, 0};
    Floor    terrain_[kTerrainMaxLevels];
};  // class RegionComponent

class ViewportComponent {
public:
    DEF_VAL_PROP_RW(Vertex2i, center_coord);
    DEF_VAL_PROP_RW(Vertex2i, bound);

private:
    Vertex2i center_coord_ = {0, 0};
    Vertex2i bound_        = {kDefaultViewportSize, kDefaultViewportSize};
};  // class ViewportComponent

class ZoneComponent {
public:
    enum Want { kNone, kE, kS, kW, kN, kSE, kNE, kSW, kNW };

    ZoneComponent();
    ~ZoneComponent();

    CubeComponent *CubeAt(int i, int j, int z);

    CubeComponent *Cube(int x, int y, int z);

    Want UpdateViewportCoord(Vertex2i coord) {
        viewport_.set_center_coord(coord);
        want_ = WantSibling();
        return want_;
    }

    Want WantSibling();

    DEF_VAL_PROP_RM(com::ViewportComponent, viewport);
    DEF_PTR_PROP_RW(com::RegionComponent, region);

    com::RegionComponent *sibling(int i) {
        DCHECK_GE(i, 0);
        DCHECK_LT(i, arraysize(sibling_));
        return sibling_[i];
    }

    void set_sibling(int i, com::RegionComponent *region) {
        DCHECK_GE(i, 0);
        DCHECK_LT(i, arraysize(sibling_));
        sibling_[i] = region;
    }

    DISALLOW_IMPLICIT_CONSTRUCTORS(ZoneComponent);

private:
    com::ViewportComponent viewport_;
    Want want_ = kNone;
    com::RegionComponent * region_ = nullptr;
    com::RegionComponent * sibling_[4];
};  // class Zone

}  // namespace com

}  // namespace nyaa

#endif  // NYAA_COMPONENT_REGION_COMPONENT_H_