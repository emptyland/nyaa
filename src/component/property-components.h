#pragma once
#ifndef NYAA_COMPONENT_PROPERTY_COMPONENTS_H_
#define NYAA_COMPONENT_PROPERTY_COMPONENTS_H_

#include "resource/text-def-inl.h"
#include "game/vector.h"
#include "base/base.h"
#include <string>

namespace nyaa {

namespace com {

class CorePropertiesComponent {
public:
    DEF_VAL_PROP_RW(int, max_hp);
    DEF_VAL_PROP_RW(int, max_sp);
    DEF_VAL_PROP_RW(int, attack);
    DEF_VAL_PROP_RW(int, defense);
    DEF_VAL_PROP_RW(int, strength);
    DEF_VAL_PROP_RW(int, agile);

private:
    int max_hp_ = 0;
    int max_sp_ = 0;
    int attack_ = 0;
    int defense_ = 0;
    int strength_ = 0;
    int agile_ = 0;
};  // class CorePropertiesComponent

class NaturePropertiesComponent {
public:
    DEF_VAL_PROP_RW(res::TextID, name_id);
    DEF_VAL_PROP_RW(std::string, name);
    DEF_VAL_PROP_RW(int, yi);
    DEF_VAL_PROP_RW(int, camp);
    DEF_VAL_PROP_RW(float, jump_speed);
    DEF_VAL_PROP_RW(float, move_speed);

private:
    res::TextID name_id_ = res::MAX_TEXT_ID;
    std::string name_;

    int   yi_ = 0;
    int   camp_ = 0;
    float jump_speed_ = 0.0;
    float move_speed_ = 0.0;
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