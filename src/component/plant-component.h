#pragma once
#ifndef NYAA_COMPONENT_PLANT_COMPONENT_H_
#define NYAA_COMPONENT_PLANT_COMPONENT_H_

#include "resource/sprite-library.h"
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
    DEF_VAL_PROP_RW(double, ts);
    DEF_PTR_PROP_RW(res::Sprite, sprite);

    int IndexFrame(double delta) {
        ts_ += delta * sprite()->speed();
        if (ts_ >= sprite()->frames_count() + 1) { ts_ = 0; }
        return static_cast<int>(ts_) % sprite()->frames_count();
    }

    res::Texture *GetFrame(double delta) { return sprite()->frame(IndexFrame(delta)); }

    int round_down_position_x() const { return static_cast<int>(position().x); }
    int round_down_position_y() const { return static_cast<int>(position().y); }

private:
    Vector3f     position_ = {0, 0, 0};
    double       ts_ = 0;
    res::Sprite *sprite_ = nullptr;
};  // class PlantComponent

}  // namespace com

}  // namespace nyaa

#endif  // NYAA_COMPONENT_PLANT_COMPONENT_H_