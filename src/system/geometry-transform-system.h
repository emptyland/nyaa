#pragma once
#ifndef NYAA_SYSTEM_GEOMETRY_TRANSFORM_SYSTEM_H_
#define NYAA_SYSTEM_GEOMETRY_TRANSFORM_SYSTEM_H_

#include "system/system.h"
#include "game/constants.h"
#include "game/vector.h"
#include "game/matrix.h"

namespace nyaa {

namespace sys {

class GeometryTransformSystem : public base::ArenaObject {
public:
    GeometryTransformSystem() = default;

    void Enter2DProjection();
    void Exit2DProjection();

    Vector4f TransformToScreen(const Matrix<float> &projection, const Matrix<float> &view, const Matrix<float> &model,
                               const Vector4f &coord, const Vector2i viewport);

    DISALLOW_IMPLICIT_CONSTRUCTORS(GeometryTransformSystem);

private:
};  // class GeometryTransformSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_SYSTEM_GEOMETRY_TRANSFORM_SYSTEM_H_