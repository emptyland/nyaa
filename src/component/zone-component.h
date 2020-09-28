#pragma once
#ifndef NYAA_COMPONENT_REGION_COMPONENT_H_
#define NYAA_COMPONENT_REGION_COMPONENT_H_

#include "component/cube-component.h"
#include "component/plant-component.h"
#include "game/constants.h"
#include <vector>

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

    DEF_VAL_PROP_RM(std::vector<PlantComponent>, plants);

    size_t plants_size() const { return plants().size(); }

    PlantComponent *plant(int i) {
        DCHECK_GE(i, 0);
        DCHECK_LT(i, plants_size());
        return &plants_[i];
    }

private:
    Vector2i                    global_coord_ = {0, 0};
    Floor                       terrain_[kTerrainMaxLevels];
    std::vector<PlantComponent> plants_;
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
    ZoneComponent();
    ~ZoneComponent();

    CubeComponent *Cube(int x, int y, int z);

    DEF_VAL_PROP_RM(ViewportComponent, viewport);
    DEF_VAL_PROP_RM(RegionComponent, region);
    DEF_VAL_PROP_RW(float, gravity);

    DISALLOW_IMPLICIT_CONSTRUCTORS(ZoneComponent);

private:
    ViewportComponent viewport_;
    RegionComponent   region_;
    float             gravity_ = 0;
};  // class Zone

}  // namespace com

}  // namespace nyaa

#endif  // NYAA_COMPONENT_REGION_COMPONENT_H_