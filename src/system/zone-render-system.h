#pragma once
#ifndef NYAA_ZONE_RENDER_SYSTEM_H_
#define NYAA_ZONE_RENDER_SYSTEM_H_

#include "game/constants.h"
#include "base/base.h"

namespace nyaa {
namespace com {
class ZoneComponent;
}  // namespace com
namespace res {
class Cube;
class Texture;
}  // namespace res

namespace sys {

class ZoneRenderSystem {
public:
    ZoneRenderSystem() = default;

    DEF_VAL_PROP_RW(float, cube_size);
    DEF_VAL_PROP_RW(uint32_t, tile_tex_id);

    void RenderTerrain(com::ZoneComponent *zone);
    void RenderPlantLayout(com::ZoneComponent *zone, int layout);

    DISALLOW_IMPLICIT_CONSTRUCTORS(ZoneRenderSystem);

private:
    void RenderSurface(com::ZoneComponent *zone, int i, int j);
    void RenderPlant(res::Cube *def, Vertex3f p0, res::Texture *tex);

    float    cube_size_   = 1;
    uint32_t tile_tex_id_ = -1;
};  // class ZoneRenderSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_ZONE_RENDER_SYSTEM_H_