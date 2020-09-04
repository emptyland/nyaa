#include "resource/skill-library.h"
#include "resource/sprite-library.h"
#include "resource/sprite-library.h"
#include "resource/text-library.h"

namespace nyaa {

namespace res {

class SkillDef : public Definition<SkillDef> {
public:
    DEF_VAL_GETTER(ResourceId, id);
    DEF_VAL_GETTER(std::string, name);
    DEF_VAL_GETTER(ResourceId, icon);
    DEF_VAL_GETTER(float, range);
    DEF_VAL_GETTER(float, speed_addition);
    DEF_VAL_GETTER(float, attack_addition);
    DEF_VAL_GETTER(float, crit_addition);

    void Parse(const std::vector<std::string_view> items) {
        int i = 0;
        CHECKED_PARSE(ID, id_);
        CHECKED_PARSE(STRING, name_);
        CHECKED_PARSE(ID, icon_);
        CHECKED_PARSE(STRING, weapon_required_);
        CHECKED_PARSE(F32, range_);
        CHECKED_PARSE(F32, speed_addition_);
        CHECKED_PARSE(F32, attack_addition_);
        CHECKED_PARSE(F32, crit_addition_);
    }

private:
    ResourceId  id_;
    std::string name_;
    ResourceId  icon_;
    std::string weapon_required_;
    float       range_;
    float       speed_addition_;
    float       attack_addition_;
    float       crit_addition_;
};  // class SkillDef

const char SkillLibrary::kSkillDir[] = "actor";
const char SkillLibrary::kSkillDefFileName[] = "actor/skill.txt";

SkillLibrary::SkillLibrary(const SpriteLibrary *sprite_lib, const TextLibrary *text_lib, base::Arena *arena)
    : ResourceLibrary<Skill, SkillLibrary>(arena), sprite_lib_(sprite_lib), text_lib_(text_lib) {}

bool SkillLibrary::Load(DefinitionReader *rd) {
    SkillDef row;
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

        Sprite *icon = sprite_lib_->FindOrNull(row.icon());
        if (row.icon().value() != 0 && !icon) {
            DLOG(ERROR) << "Sprite id: " << row.icon().value() << " not found!";
            return false;
        }

        Skill *skill = new (arena()) Skill(row.id(), name_id, text_lib_->Load(name_id), icon);
        skill->weapon_requried_ = 0;
        skill->range_ = row.range();
        skill->speed_addition_ = row.speed_addition();
        skill->attack_addition_ = row.attack_addition();
        skill->crit_addition_ = row.crit_addition();
        Put(skill->id(), skill);
    }
    return true;
}

}  // namespace res

}  // namespace nyaa
