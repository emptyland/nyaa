#pragma once
#ifndef NYAA_COMPONENT_PROPERTY_COMPONENTS_H_
#define NYAA_COMPONENT_PROPERTY_COMPONENTS_H_

#include "game/vector.h"
#include "base/base.h"

namespace nyaa {

namespace com {

class NaturePropertiesComponent {
public:
    DEF_VAL_PROP_RW(int, yi);
    DEF_VAL_PROP_RW(int, camp);
    DEF_VAL_PROP_RW(float, jump_speed);
    DEF_VAL_PROP_RW(float, move_speed);

private:
    int   yi_;
    int   camp_;
    float jump_speed_;
    float move_speed_;
};  // class NaturePropertiesComponent

class NPCNaturePropertiesComponent : public NaturePropertiesComponent {
public:
    DEF_VAL_PROP_RW(int, difficulty);
    DEF_VAL_PROP_RW(Vector3f, spawn_point);
    DEF_VAL_PROP_RW(float, patrol_radius);

private:
    int      difficulty_    = 0;
    Vector3f spawn_point_   = {0, 0, 0};
    float    patrol_radius_ = 0;
};  // class NPCPropertiesComponent

}  // namespace com

}  // namespace nyaa

#endif  // NYAA_COMPONENT_PROPERTY_COMPONENTS_H_