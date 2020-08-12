#pragma once
#ifndef NYAA_ENTITY_CUBE_ENTITY_H_
#define NYAA_ENTITY_CUBE_ENTITY_H_

#include "resource/cube-library.h"
#include "entity/entity.h"

namespace nyaa {

namespace entity {

class CubeEntity final : public Entity<CubeEntity> {
public:
    using Kind = res::Cube::Kind;

    CubeEntity() = default;

    DEF_VAL_PROP_RW(Kind, kind);
    DEF_VAL_PROP_RW(int, hardness);

    res::Texture *GetTopTexture() const;
    res::Texture *GetEdgeTexture() const;

private:
    Kind kind_     = res::Cube::CUBE_WOOD_1;
    int  hardness_ = 0;
};  // class CubeEntity

}  // namespace entity

}  // namespace nyaa

#endif  // NYAA_ENTITY_CUBE_ENTITY_H_