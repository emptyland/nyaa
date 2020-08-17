#pragma once
#ifndef NYAA_ZONE_RENDER_SYSTEM_H_
#define NYAA_ZONE_RENDER_SYSTEM_H_

#include "game/constants.h"
#include "base/base.h"

namespace nyaa {
namespace com {
class ZoneComponent;
}  // namespace com

namespace sys {

class ZoneRenderSystem {
public:
    ZoneRenderSystem() = default;

    DEF_VAL_PROP_RW(double, rotate_angle_y);
    DEF_VAL_PROP_RW(double, rotate_angle_z);
    DEF_VAL_PROP_RW(double, scale);
    DEF_VAL_PROP_RW(float, cube_size);
    DEF_VAL_PROP_RW(uint32_t, tile_tex_id);

    void Render(com::ZoneComponent *zone);

    DISALLOW_IMPLICIT_CONSTRUCTORS(ZoneRenderSystem);
private:
    void RenderSurface(com::ZoneComponent *zone, int i, int j);

    double rotate_angle_y_ = kDefaultZoneRotateAngleY;
    double rotate_angle_z_ = kDefaultZoneRotateAngleZ;
    double scale_ = kDefaultZoneScale;
    float cube_size_ = 1;
    uint32_t tile_tex_id_ = -1;
};  // class ZoneRenderSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_ZONE_RENDER_SYSTEM_H_