#pragma once
#ifndef NYAA_RESOURCE_ACTOR_LIBRARY_H_
#define NYAA_RESOURCE_ACTOR_LIBRARY_H_

#include "resource/resource-library.h"
#include "resource/ai-def.h"
#include "resource/text-def-inl.h"
#include "game/vector.h"

namespace nyaa {

namespace res {

class TextLibrary;
class AvatarLibrary;
class Avatar;

class Actor : public base::ArenaObject {
public:
    DEF_VAL_GETTER(ResourceId, id);
    DEF_VAL_GETTER(res::TextID, name_id);
    DEF_PTR_GETTER(const char, name);
    DEF_PTR_GETTER(Avatar, avatar);

    DEF_VAL_GETTER(AI::Kind, ai);
    DEF_VAL_GETTER(int, difficulty);
    DEF_VAL_GETTER(int, yi);
    DEF_VAL_GETTER(int, camp);

    DEF_VAL_GETTER(float, jump_speed);
    DEF_VAL_GETTER(float, move_speed);
    DEF_VAL_GETTER(float, patrol_radius);

    DEF_VAL_GETTER(int, max_hp);
    DEF_VAL_GETTER(int, max_sp);
    DEF_VAL_GETTER(int, attack);
    DEF_VAL_GETTER(int, defense);
    DEF_VAL_GETTER(int, strength);
    DEF_VAL_GETTER(int, agile);

    friend class ActorLibrary;
    DISALLOW_IMPLICIT_CONSTRUCTORS(Actor);

private:
    Actor(ResourceId id, res::TextID name_id, const char *name, Avatar *avatar)
        : id_(id), name_id_(name_id), name_(name), avatar_(avatar) {}

    const ResourceId  id_;
    const res::TextID name_id_;
    const char *const name_;
    Avatar *const     avatar_;

    AI::Kind ai_;
    int      difficulty_;
    int      yi_;
    int      camp_;
    float    jump_speed_;
    float    move_speed_;
    float    patrol_radius_;
    int      max_hp_;
    int      max_sp_;
    int      attack_;
    int      defense_;
    int      strength_;
    int      agile_;
};  // class Actor

class ActorLibrary : public ResourceLibrary<Actor, ActorLibrary> {
public:
    static const char kActorDir[];
    static const char kActorDefFileName[];

    ActorLibrary(const AvatarLibrary *avatar_lib, const TextLibrary *text_lib, base::Arena *arena);

    bool Load(DefinitionReader *rd);

    DISALLOW_IMPLICIT_CONSTRUCTORS(ActorLibrary);

private:
    const AvatarLibrary *const avatar_lib_;
    const TextLibrary *const   text_lib_;
};  // class ActorLibrary

}  // namespace res

}  // namespace nyaa

#endif  // NYAA_RESOURCE_ACTOR_LIBRARY_H_