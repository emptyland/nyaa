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

    nature_properties_.set_camp(def()->camp());
    nature_properties_.set_difficulty(def()->difficulty());
    nature_properties_.set_yi(def()->yi());
}

}  // namespace entity

}  // namespace nyaa
