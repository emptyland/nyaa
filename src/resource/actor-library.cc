#include "resource/actor-library.h"
#include "resource/text-library.h"
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

    void Parse(const std::vector<std::string_view> items) {
        ParseValue<DefValType::ID>(items[0], &id_);
        ParseValue<DefValType::STRING>(items[1], &name_);
        ParseValue<DefValType::ID>(items[2], &avatar_);
        ParseValue<DefValType::I32>(items[3], &difficulty_);
        ParseValue<DefValType::I32>(items[4], &yi_);
        ParseValue<DefValType::I32>(items[5], &camp_);
        ParseValue<DefValType::STRING>(items[6], &ai_);
        // TODO:
    }

private:
    ResourceId  id_;
    std::string name_;
    ResourceId  avatar_;
    int         difficulty_ = 0;
    int         yi_         = 0;
    int         camp_       = 0;
    std::string ai_;
};  // class ActorDef

const char ActorLibrary::kActorDir[]         = "actor";
const char ActorLibrary::kActorDefFileName[] = "actor/def.txt";

ActorLibrary::ActorLibrary(const AvatarLibrary *avatar_lib, const TextLibrary *text_lib, base::Arena *arena)
    : ResourceLibrary(arena), avatar_lib_(avatar_lib), text_lib_(text_lib) {}

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

        Actor *actor       = new (arena()) Actor(row.id(), name_id, text_lib_->Load(name_id).data(), avatar);
        actor->yi_         = row.yi();
        actor->difficulty_ = row.difficulty();
        actor->camp_       = row.camp();
        actor->ai_         = ai;

        Put(row.id(), actor);
    }

    return true;
}

}  // namespace res

}  // namespace nyaa