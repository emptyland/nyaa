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

    void Prepare();

    void RenderTerrain(com::ZoneComponent *zone);

    DISALLOW_IMPLICIT_CONSTRUCTORS(ZoneRenderSystem);

private:
    void RenderSurface(com::ZoneComponent *zone, int i, int j);

    void MakeCube(const Vector3f &p0);

    struct Bundle {
        Vector3f vertex;
        Vector3f normal;
        Vector2f uv;
    };
    static_assert(sizeof(Bundle) == 8 * sizeof(float), "Incorrect bundle size");

    float    cube_size_   = 1;
    uint32_t tile_tex_id_ = -1;
    uint32_t vao_ = 0;
    uint32_t vbo_ = 0;
    Bundle vertices_[24];
};  // class ZoneRenderSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_ZONE_RENDER_SYSTEM_H_