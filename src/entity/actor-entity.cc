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

    nature_properties_.set_jump_speed(def()->jump_speed());
    nature_properties_.set_move_speed(def()->move_speed());
    nature_properties_.set_patrol_radius(def()->patrol_radius());

    base_properties_.set_max_hp(def()->max_hp());
    base_properties_.set_max_sp(def()->max_sp());
    base_properties_.set_attack(def()->attack());
    base_properties_.set_defense(def()->defense());
    base_properties_.set_strength(def()->strength());
    base_properties_.set_agile(def()->agile());

    ::memcpy(&core_properties_, &base_properties_, sizeof(core_properties_));

    nature_properties_.set_name_id(def()->name_id());
    if (def()->name() != nullptr) { nature_properties_.set_name(def()->name()); }
    nature_properties_.set_camp(def()->camp());
    nature_properties_.set_difficulty(def()->difficulty());
    nature_properties_.set_yi(def()->yi());
}

}  // namespace entity

}  // namespace nyaa
