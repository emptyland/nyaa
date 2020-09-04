#include "resource/actor-library.h"
#include "resource/text-library.h"
#include "resource/skill-library.h"
#include "resource/avatar-library.h"
#include "resource/definition.h"

namespace nyaa {

namespace res {

class ActorDef : public Definition<ActorDef> {
public:
    DEF_VAL_GETTER(ResourceId, id);
    DEF_VAL_GETTER(std::string, name);
    DEF_VAL_GETTER(ResourceId, avatar);
    DEF_VAL_GETTER(int, difficulty);
    DEF_VAL_GETTER(int, yi);
    DEF_VAL_GETTER(int, camp);
    DEF_VAL_GETTER(std::string, ai);

    DEF_VAL_GETTER(float, jump_speed);
    DEF_VAL_GETTER(float, move_speed);
    DEF_VAL_GETTER(float, patrol_radius);

    DEF_VAL_GETTER(int, max_hp);
    DEF_VAL_GETTER(int, max_sp);
    DEF_VAL_GETTER(int, attack);
    DEF_VAL_GETTER(int, defense);
    DEF_VAL_GETTER(int, strength);
    DEF_VAL_GETTER(int, agile);

    DEF_VAL_GETTER(int, skill_count);
    ResourceId skill(int i) const {
        DCHECK_GE(i, 0);
        DCHECK_LT(i, skill_count());
        return skills_[i];
    }

    void Parse(const std::vector<std::string_view> items) {
        int i = 0;
        CHECKED_PARSE(ID, id_);
        CHECKED_PARSE(STRING, name_);
        CHECKED_PARSE(ID, avatar_);
        CHECKED_PARSE(I32, difficulty_);
        CHECKED_PARSE(I32, yi_);
        CHECKED_PARSE(I32, camp_);
        CHECKED_PARSE(STRING, ai_);
        CHECKED_PARSE(F32, jump_speed_);
        CHECKED_PARSE(F32, move_speed_);
        CHECKED_PARSE(F32, patrol_radius_);
        CHECKED_PARSE(I32, max_hp_);
        CHECKED_PARSE(I32, max_sp_);
        CHECKED_PARSE(I32, attack_);
        CHECKED_PARSE(I32, defense_);
        CHECKED_PARSE(I32, strength_);
        CHECKED_PARSE(I32, agile_);
        CHECKED_PARSE(ARRAY_U32, skill_count_);
    }

private:
    ResourceId  id_;
    std::string name_;
    ResourceId  avatar_;
    int         difficulty_ = 0;
    int         yi_         = 0;
    int         camp_       = 0;
    std::string ai_;
    float       jump_speed_    = 0.0;
    float       move_speed_    = 0.0;
    float       patrol_radius_ = 0.0;
    int         max_hp_        = 0;
    int         max_sp_        = 0;
    int         attack_        = 0;
    int         defense_       = 0;
    int         strength_      = 0;
    int         agile_         = 0;
    int         skill_count_   = 0;
    ResourceId  skills_[Actor::kMaxSkills];
};  // class ActorDef

Actor::Actor(ResourceId id, res::TextID name_id, const char *name, Avatar *avatar)
    : id_(id), name_id_(name_id), name_(name), avatar_(avatar) {
    ::memset(skills_, 0, sizeof(skills_));
}

const char ActorLibrary::kActorDir[]         = "actor";
const char ActorLibrary::kActorDefFileName[] = "actor/def.txt";

ActorLibrary::ActorLibrary(const AvatarLibrary *avatar_lib, const SkillLibrary *skill_lib, const TextLibrary *text_lib,
                           base::Arena *arena)
    : ResourceLibrary(arena), avatar_lib_(avatar_lib), skill_lib_(skill_lib), text_lib_(text_lib) {}

bool ActorLibrary::Load(DefinitionReader *rd) {
    ActorDef row;
    while (row.Read(rd) != EOF) {
        if (FindOrNull(row.id())) {
            DLOG(ERROR) << "Duplicated actor id: " << row.id().value();
            return false;
        }
        TextID name_id = text_lib_->FindID(row.name());
        if (name_id == MAX_TEXT_ID) {
            DLOG(ERROR) << "Name id: " << row.name() << " not found!";
            return false;
        }

        Avatar *avatar = avatar_lib_->FindOrNull(row.avatar());
        if (!avatar) {
            DLOG(ERROR) << "Can not find avatar: " << row.avatar().value();
            return false;
        }

        AI::Kind ai = AI::OfName(row.ai());
        if (ai == AI::MAX_AI_KIND) {
            DLOG(ERROR) << "Can not find ai kind: " << row.ai();
            return false;
        }

        Skill *skills[Actor::kMaxSkills] = {nullptr};
        for (int i = 0; i < row.skill_count(); i++) {
            if (skills[i] = skill_lib_->FindOrNull(row.skill(i)); !skills[i]) {
                DLOG(ERROR) << "Can not find skill: " << row.skill(i).value();
                return false;
            }
        }

        Actor *actor = new (arena()) Actor(row.id(), name_id, text_lib_->Load(name_id).data(), avatar);

        actor->yi_            = row.yi();
        actor->difficulty_    = row.difficulty();
        actor->camp_          = row.camp();
        actor->ai_            = ai;
        actor->jump_speed_    = row.jump_speed();
        actor->move_speed_    = row.move_speed();
        actor->patrol_radius_ = row.patrol_radius();
        actor->max_hp_        = row.max_hp();
        actor->max_sp_        = row.max_sp();
        actor->attack_        = row.attack();
        actor->defense_       = row.defense();
        actor->strength_      = row.strength();
        actor->agile_         = row.agile();
        actor->skill_count_   = row.skill_count();
        ::memcpy(actor->skills_, skills, sizeof(skills));

        Put(row.id(), actor);
    }

    return true;
}

}  // namespace res

}  // namespace nyaa