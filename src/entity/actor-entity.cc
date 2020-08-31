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
}

}  // namespace entity

}  // namespace nyaa
