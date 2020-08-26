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

    DEF_VAL_PROP_RW(Vector2i, global_coord);

    bool GlobalCoordEqual(int x, int y) const { return global_coord().x == x && global_coord().y == y; }

    bool AtEast(int x, int y) const {
        return x >= global_coord().x + kRegionSize && y < global_coord().y + kRegionSize && y >= global_coord().y;
    }
    bool AtWest(int x, int y) const {
        return x < global_coord().x && y < global_coord().y + kRegionSize && y >= global_coord().y;
    }
    bool AtSouth(int x, int y) const {
        return y >= global_coord().y + kRegionSize && x >= global_coord().x && x < global_coord().x + kRegionSize;
    }
    bool AtNorth(int x, int y) const {
        return y < global_coord().y && x >= global_coord().x && x < global_coord().x + kRegionSize;
    }

    bool AtSouthEast(int x, int y) const {
        return x >= global_coord().x + kRegionSize && y >= global_coord().y + kRegionSize;
    }
    bool AtSouthWest(int x, int y) const { return x < global_coord().x && y >= global_coord().y + kRegionSize; }
    bool AtNorthEast(int x, int y) const { return x >= global_coord().x + kRegionSize && y < global_coord().y; }
    bool AtNorthWest(int x, int y) const { return x < global_coord().x && y < global_coord().y; }

    bool AtBound(int x, int y) const {
        return x >= global_coord().x && x < global_coord().x + kRegionSize && y >= global_coord().y &&
               y < global_coord().y + kRegionSize;
    }

    Floor *floor(int i) {
        DCHECK_GE(i, 0);
        DCHECK_LT(i, kTerrainMaxLevels);
        return &terrain_[i];
    }

private:
    Vector2i global_coord_ = {0, 0};
    Floor    terrain_[kTerrainMaxLevels];
};  // class RegionComponent

class ViewportComponent {
public:
    DEF_VAL_PROP_RMW(Vector2f, center_coord);
    DEF_VAL_PROP_RW(Vector2i, bound);

    int round_down_center_x() const { return static_cast<int>(center_coord().x); }
    int round_down_center_y() const { return static_cast<int>(center_coord().y); }

    float adjust_center_x() const { return center_coord().x - round_down_center_x(); }
    float adjust_center_y() const { return center_coord().y - round_down_center_y(); }

private:
    Vector2f center_coord_ = {0, 0};
    Vector2i bound_        = {kDefaultViewportSize, kDefaultViewportSize};
};  // class ViewportComponent

class ZoneComponent {
public:
    enum Direction { kNone, kE, kS, kW, kN, kSE, kNE, kSW, kNW };

    ZoneComponent();
    ~ZoneComponent();

    CubeComponent *CubeAt(int i, int j, int z);

    CubeComponent *Cube(int x, int y, int z);

    void UpdateViewportCoord(Vector2f coord) {
        viewport_.set_center_coord(coord);
        want_ = WantSibling();
    }

    void UpdateViewportCoord() { want_ = WantSibling(); }

    Direction WantSibling();

    DEF_VAL_PROP_RM(com::ViewportComponent, viewport);
    DEF_VAL_GETTER(Direction, want);

    com::RegionComponent *center() const { return regions_[1][1]; }

    const char *want_string() const { return kDirectionText[want()]; }

    com::RegionComponent *region(int i, int j) {
        DCHECK_GE(i, 0);
        DCHECK_LT(i, 3);
        DCHECK_GE(j, 0);
        DCHECK_LT(j, 3);
        return regions_[i][j];
    }

    void set_region(int i, int j, com::RegionComponent *region) {
        DCHECK_GE(i, 0);
        DCHECK_LT(i, 3);
        DCHECK_GE(j, 0);
        DCHECK_LT(j, 3);
        regions_[i][j] = region;
    }

    void ClearRegions() {
        for (int i = 0; i < 3; i++) {
            regions_[i][0] = nullptr;
            regions_[i][1] = nullptr;
            regions_[i][2] = nullptr;
        }
    }

    DISALLOW_IMPLICIT_CONSTRUCTORS(ZoneComponent);

private:
    com::ViewportComponent viewport_;
    com::RegionComponent * regions_[3][3];

    Direction want_ = kNone;

    static const char *kDirectionText[];
};  // class Zone

}  // namespace com

}  // namespace nyaa

#endif  // NYAA_COMPONENT_REGION_COMPONENT_H_