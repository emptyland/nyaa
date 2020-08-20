#pragma once
#ifndef NYAA_SYSTEM_GEOMETRY_TRANSFORM_SYSTEM_H_
#define NYAA_SYSTEM_GEOMETRY_TRANSFORM_SYSTEM_H_

#include "game/constants.h"
#include "base/base.h"

namespace nyaa {

namespace sys {

class GeometryTransformSystem {
public:
    GeometryTransformSystem() = default;

    DEF_VAL_PROP_RW(double, rotate_angle_y);
    DEF_VAL_PROP_RW(double, rotate_angle_z);
    DEF_VAL_PROP_RW(double, scale);

    void Enter2DProjection();
    void Exit2DProjection();

    void EnterRotatedProjection();
    void ExitRotatedProjection();

    DISALLOW_IMPLICIT_CONSTRUCTORS(GeometryTransformSystem);

private:
    double rotate_angle_y_ = kDefaultZoneRotateAngleY;
    double rotate_angle_z_ = kDefaultZoneRotateAngleZ;
    double scale_          = kDefaultZoneScale;
};  // class GeometryTransformSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_SYSTEM_GEOMETRY_TRANSFORM_SYSTEM_H_