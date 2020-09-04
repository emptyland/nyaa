#pragma once
#ifndef NYAA_RESOURCE_SKILL_LIBRARY_H_
#define NYAA_RESOURCE_SKILL_LIBRARY_H_

#include "resource/resource-library.h"
#include "resource/text-def-inl.h"
#include "base/base.h"
#include <string_view>

namespace nyaa {

namespace res {

class SpriteLibrary;
class Sprite;
class TextLibrary;

class Skill : public base::ArenaObject {
public:
    DEF_VAL_GETTER(ResourceId, id);
    DEF_VAL_GETTER(res::TextID, name_id);
    DEF_VAL_GETTER(std::string_view, name);
    DEF_PTR_GETTER(Sprite, icon);

    DEF_VAL_GETTER(int, weapon_requried);
    DEF_VAL_GETTER(float, range);
    DEF_VAL_GETTER(float, speed_addition);
    DEF_VAL_GETTER(float, attack_addition);
    DEF_VAL_GETTER(float, crit_addition);

    friend class SkillLibrary;
    DISALLOW_IMPLICIT_CONSTRUCTORS(Skill);

private:
    Skill(ResourceId id, res::TextID name_id, std::string_view name, Sprite *icon)
        : id_(id), name_id_(name_id), name_(name), icon_(icon) {}

    const ResourceId       id_;
    const res::TextID      name_id_;
    const std::string_view name_;
    Sprite *const          icon_;

    int   weapon_requried_ = 0;
    float range_           = 0;
    float speed_addition_  = 0;
    float attack_addition_ = 0;
    float crit_addition_   = 0;
};  // class Skill

class SkillLibrary : public ResourceLibrary<Skill, SkillLibrary> {
public:
    static const char kSkillDir[];
    static const char kSkillDefFileName[];

    SkillLibrary(const SpriteLibrary *sprite_lib, const TextLibrary *text_lib, base::Arena *arena);

    bool Load(DefinitionReader *rd);

    DISALLOW_IMPLICIT_CONSTRUCTORS(SkillLibrary);

private:
    const SpriteLibrary *const sprite_lib_;
    const TextLibrary *const   text_lib_;
};  // class SkillLibrary

}  // namespace res

}  // namespace nyaa

#endif  // NYAA_RESOURCE_SKILL_LIBRARY_H_