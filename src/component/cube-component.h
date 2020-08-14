#pragma once
#ifndef NYAA_COMPONENT_CUBE_COMPONENT_H_
#define NYAA_COMPONENT_CUBE_COMPONENT_H_

#include "resource/cube-library.h"

namespace nyaa {

namespace com {

class CubeComponent {
public:
    using Kind = res::Cube::Kind;

    DEF_VAL_PROP_RW(Kind, kind);
    DEF_VAL_PROP_RW(int, hardness);

    res::Texture *GetTopTexture() const;
    res::Texture *GetEdgeTexture() const;

private:
    Kind kind_     = res::Cube::CUBE_AIR;
    int  hardness_ = 0;
};  // class CubeComponent

template<int W, int H>
struct FloorComponent {
    CubeComponent cubes[W][H];
}; // struct FloorComponent

}  // namespace com

}  // namespace nyaa

#endif  // NYAA_COMPONENT_CUBE_COMPONENT_H_