#pragma once
#ifndef NYAA_ZONE_RENDER_SYSTEM_H_
#define NYAA_ZONE_RENDER_SYSTEM_H_

#include "game/constants.h"
#include "base/base.h"
#include <vector>

namespace nyaa {
namespace com {
class ZoneComponent;
class RegionComponent;
}  // namespace com
namespace res {
class Cube;
class Texture;
}  // namespace res

namespace sys {

class ZoneRenderSystem {
public:
    ZoneRenderSystem();

    DEF_VAL_PROP_RW(float, cube_size);
    DEF_VAL_PROP_RW(uint32_t, tile_tex_id);

    void Prepare();

    void RenderTerrain(com::ZoneComponent *zone);

    DISALLOW_IMPLICIT_CONSTRUCTORS(ZoneRenderSystem);

private:
    void RenderSurface(com::ZoneComponent *zone, int i, int j);

    void GenBuffer(com::RegionComponent *region, int i, int j);

    void MakeCube(const res::Cube *cube, const Vector3f &p0, std::vector<float> *buf);

    struct Vbo {
        Vector2i coord;
        uint32_t buffer;
        int count;
    };

    float    cube_size_   = 1;
    uint32_t tile_tex_id_ = -1;
    Vbo vbo_[3][3];
};  // class ZoneRenderSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_ZONE_RENDER_SYSTEM_H_