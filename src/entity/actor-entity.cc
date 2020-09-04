#include "entity/actor-entity.h"
#include "resource/actor-library.h"
#include "resource/avatar-library.h"
#include "game/game.h"

namespace nyaa {

namespace entity {

ActorEntity::ActorEntity(EntityId id, ResourceId actor_id)
    : Entity(id, Class<ActorEntity>::id())
    , def_(Game::This()->actor_lib()->FindOrNull(actor_id))
    , avatar_(def_->avatar())
    , ai_state_(def_->ai()) {
    DCHECK(def() != nullptr);
    DCHECK(avatar().def() != nullptr);

    nature_.set_jump_speed(def()->jump_speed());
    nature_.set_move_speed(def()->move_speed());
    nature_.set_patrol_radius(def()->patrol_radius());

    core_base_.set_max_hp(def()->max_hp());
    core_base_.set_max_sp(def()->max_sp());
    core_base_.set_attack(def()->attack());
    core_base_.set_defense(def()->defense());
    core_base_.set_strength(def()->strength());
    core_base_.set_agile(def()->agile());

    ::memcpy(&core_, &core_base_, sizeof(core_));

    nature_.set_name_id(def()->name_id());
    if (def()->name() != nullptr) { nature_.set_name(def()->name()); }
    nature_.set_camp(def()->camp());
    nature_.set_difficulty(def()->difficulty());
    nature_.set_yi(def()->yi());
}

}  // namespace entity

}  // namespace nyaa
