#pragma once
#ifndef NYAA_COMPONENT_MOVEMENT_COMPONENT_H_
#define NYAA_COMPONENT_MOVEMENT_COMPONENT_H_

#include "base/base.h"

namespace nyaa {

namespace com {

using PositionComponent = Vertex3f;
using VelocityComponent = Vertex3f;

class MovementComponent {
public:
    DEF_VAL_PROP_RMW(PositionComponent, coord);
    DEF_VAL_PROP_RMW(VelocityComponent, speed);

private:
    PositionComponent coord_;
    VelocityComponent speed_; // MPS
}; // class MovementComponent

}  // namespace com

}  // namespace nyaa

#endif  // NYAA_COMPONENT_MOVEMENT_COMPONENT_H_