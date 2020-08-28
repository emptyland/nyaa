#pragma once
#ifndef NYAA_COMPONENT_PLANT_COMPONENT_H_
#define NYAA_COMPONENT_PLANT_COMPONENT_H_

#include "base/base.h"

namespace nyaa {
namespace res {
class Texture;
}  // namespace res

namespace com {

class PlantComponent {
public:
    PlantComponent() = default;

    DEF_VAL_PROP_RMW(Vector3f, position);
    DEF_PTR_PROP_RW(res::Texture, tex);

    int round_down_position_x() const { return static_cast<int>(position().x); }
    int round_down_position_y() const { return static_cast<int>(position().y); }

    //DISALLOW_IMPLICIT_CONSTRUCTORS(PlantComponent);

private:
    Vector3f      position_;
    res::Texture *tex_;
};  // class PlantComponent

}  // namespace com

}  // namespace nyaa

#endif  // NYAA_COMPONENT_PLANT_COMPONENT_H_