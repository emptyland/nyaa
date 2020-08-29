#pragma once
#ifndef NYAA_RESOURCE_ACTOR_LIBRARY_H_
#define NYAA_RESOURCE_ACTOR_LIBRARY_H_

#include "game/identifiers.h"
#include "game/vector.h"
#include "resource/ai-def.h"
#include "resource/text-ids-inl.h"
#include "base/arena-utils.h"
#include "base/base.h"

namespace nyaa {

namespace res {

class AvatarLibrary;
class Avatar;

class Actor : public base::ArenaObject {
public:
    Actor(ResourceId id, int name_id, const char *name, Avatar *avatar)
        : id_(id), name_id_(name_id), name_(name), avatar_(avatar) {}

    DEF_VAL_GETTER(ResourceId, id);
    DEF_VAL_GETTER(int, name_id);
    DEF_PTR_GETTER(const char, name);
    DEF_PTR_GETTER(Avatar, avatar);

    DEF_VAL_GETTER(AI::Kind, ai);
    DEF_VAL_GETTER(int, difficulty);
    DEF_VAL_GETTER(int, yi);
    DEF_VAL_GETTER(int, camp);

    friend class ActorLibrary;
    DISALLOW_IMPLICIT_CONSTRUCTORS(Actor);

private:
    const ResourceId  id_;
    const int         name_id_;
    const char *const name_;
    Avatar *const     avatar_;

    AI::Kind ai_;
    int      difficulty_;
    int      yi_;
    int      camp_;
};  // class Actor

class ActorLibrary {
public:
    static const char kActorDir[];
    static const char kActorDefFileName[];

    ActorLibrary(AvatarLibrary *avatar_lib, base::Arena *arena)
        : arena_(arena), avatar_lib_(avatar_lib), actors_(arena) {}

    const base::ArenaUnorderedMap<ResourceId, Actor *> &actors() const { return actors_; }

    bool Prepare(const std::string &file_name);

    Actor *FindOrNull(ResourceId id) const {
        auto iter = actors_.find(id);
        return iter == actors_.end() ? nullptr : iter->second;
    }

    DISALLOW_IMPLICIT_CONSTRUCTORS(ActorLibrary);

private:
    base::Arena *const   arena_;
    AvatarLibrary *const avatar_lib_;

    base::ArenaUnorderedMap<ResourceId, Actor *> actors_;
};  // class ActorLibrary

}  // namespace res

}  // namespace nyaa

#endif  // NYAA_RESOURCE_ACTOR_LIBRARY_H_