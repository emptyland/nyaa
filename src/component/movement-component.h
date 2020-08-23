#pragma once
#ifndef NYAA_COMPONENT_MOVEMENT_COMPONENT_H_
#define NYAA_COMPONENT_MOVEMENT_COMPONENT_H_

#include "base/base.h"

namespace nyaa {

namespace com {

using PositionComponent = Vector3f;
using VelocityComponent = Vector3f;

class MovementComponent {
public:
    int round_down_coord_x() const { return static_cast<int>(coord().x); }
    int round_down_coord_y() const { return static_cast<int>(coord().y); }
    int round_down_coord_z() const { return static_cast<int>(coord().z); }

    bool is_moving() const { return !is_stop(); }
    bool is_stop() const { return is_horizontal_stop() && speed().z == 0; }

    bool is_horizontal_moving() const { return !is_horizontal_stop(); }
    bool is_horizontal_stop() const { return speed().x == 0 && speed().y == 0; }

    DEF_VAL_PROP_RMW(PositionComponent, coord);
    DEF_VAL_PROP_RMW(VelocityComponent, speed);

    // [-π, π]
    // -π/2: North
    //    0: East
    //  π/2: South
    //    π: West
    float GetMovingDirection() const;

private:
    PositionComponent coord_ = {0, 0, 0};
    VelocityComponent speed_ = {0, 0, 0};  // MPS
};                                         // class MovementComponent

}  // namespace com

}  // namespace nyaa

#endif  // NYAA_COMPONENT_MOVEMENT_COMPONENT_H_