#include "entity/player-entity.h"
#include "resource/avatar-library.h"
#include "game/game.h"

namespace nyaa {

namespace entity {

PlayerEntity::PlayerEntity(EntityId id, ResourceId avatar_id)
    : Entity(id), avatar_(DCHECK_NOTNULL(Game::This()->avatar_lib()->FindOrNull(avatar_id))) {}

}  // namespace entity

}  // namespace nyaa
