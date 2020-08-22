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

    float *projection_mat() { return &projection_mat_[0]; }
    float *modelview_mat() { return &modelview_mat_[0]; }

    void Enter2DProjection();
    void Exit2DProjection();

    void EnterRotatedProjection();
    void ExitRotatedProjection();

    DISALLOW_IMPLICIT_CONSTRUCTORS(GeometryTransformSystem);

private:
    double rotate_angle_y_ = kDefaultZoneRotateAngleY;
    double rotate_angle_z_ = kDefaultZoneRotateAngleZ;
    double scale_          = kDefaultZoneScale;

    float projection_mat_[16];
    float modelview_mat_[16];
};  // class GeometryTransformSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_SYSTEM_GEOMETRY_TRANSFORM_SYSTEM_H_