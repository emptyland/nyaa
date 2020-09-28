#include "system/system.h"
#include "system/entity-allocation-system.h"
#include "system/zone-render-system.h"
#include "system/zone-loading-system.h"
#include "system/random-zone-system.h"
#include "system/actor-movement-system.h"
#include "system/actor-ai-system.h"
#include "system/actor-billboard-render-system.h"
#include "system/sprite-render-system.h"
#include "system/avatar-render-system.h"
#include "system/geometry-transform-system.h"
#include "system/zone-generating-system.h"
#include "resource/texture-library.h"
#include "game/game.h"

namespace nyaa {

namespace sys {

System::System()
    : entity_allocator_(new sys::EntityAllocationSystem())
    , zone_render_(new sys::ZoneRenderSystem())
    , zone_loader_(new sys::ZoneLoadingSystem())
    , random_zone_(new sys::RandomZoneSystem())
    , actor_movement_(new sys::ActorMovementSystem())
    , actor_ai_(new sys::ActorAISystem())
    , actor_billboard_(new sys::ActorBillboardRenderSystem())
    , sprite_render_(new sys::SpriteRenderSystem())
    , avatar_render_(new sys::AvatarRenderSystem())
    , transform_(new sys::GeometryTransformSystem()) {}

System::~System() {}

/*static*/ System *System::This() { return Game::This()->system(); }

void System::Prepare() {
    DCHECK(!initialized_);
    // Initial tiles texture id
    res::Texture *tex = DCHECK_NOTNULL(Game::This()->texture_lib()->FindOrNull(ResourceId::Of(200000)));
    zone_render()->set_tile_tex_id(tex->tex_id());
    sprite_render()->Prepare(Game::This()->sprite_lib());
    avatar_render()->Prepare(Game::This()->avatar_lib());

    initialized_ = true;
}

}  // namespace sys

}  // namespace nyaa