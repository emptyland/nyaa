#pragma once
#ifndef NYAA_SYSTEM_SYSTEM_H_
#define NYAA_SYSTEM_SYSTEM_H_

#include "base/arena.h"
#include "base/base.h"
#include <memory>

namespace nyaa {

namespace sys {

class EntityAllocationSystem;
class ZoneRenderSystem;
class RandomZoneSystem;
class WorldGeneratingSystem;
class ZoneLoadingSystem;
class WorldGeneratingSystem;
class ActorMovementSystem;
class ActorAISystem;
class ActorBillboardRenderSystem;
class AvatarRenderSystem;
class GeometryTransformSystem;
class SpriteRenderSystem;

class System final {
public:
    System(base::Arena* arena);
    ~System();

    DEF_VAL_GETTER(bool, initialized);

    void Prepare();

    DEF_PTR_GETTER(EntityAllocationSystem, entity_allocator);
    DEF_PTR_GETTER(WorldGeneratingSystem, world_generator);
    DEF_PTR_GETTER(ZoneRenderSystem, zone_render);
    DEF_PTR_GETTER(ZoneLoadingSystem, zone_loader);
    DEF_PTR_GETTER(AvatarRenderSystem, avatar_render);
    DEF_PTR_GETTER(SpriteRenderSystem, sprite_render);
    DEF_PTR_GETTER(RandomZoneSystem, random_zone);
    DEF_PTR_GETTER(ActorBillboardRenderSystem, actor_billboard);
    DEF_PTR_GETTER(ActorMovementSystem, actor_movement);
    DEF_PTR_GETTER(ActorAISystem, actor_ai);
    DEF_PTR_GETTER(GeometryTransformSystem, transform);

    static System* This();

    DISALLOW_IMPLICIT_CONSTRUCTORS(System);

private:
    // Systems
    EntityAllocationSystem*     entity_allocator_;
    WorldGeneratingSystem*      world_generator_;
    ZoneRenderSystem*           zone_render_;
    ZoneLoadingSystem*          zone_loader_;
    RandomZoneSystem*           random_zone_;
    ActorMovementSystem*        actor_movement_;
    ActorAISystem*              actor_ai_;
    ActorBillboardRenderSystem* actor_billboard_;
    SpriteRenderSystem*         sprite_render_;
    AvatarRenderSystem*         avatar_render_;
    GeometryTransformSystem*    transform_;

    bool initialized_ = false;
};  // class System

}  // namespace sys

using sys::System;

}  // namespace nyaa

#endif  // NYAA_SYSTEM_SYSTEM_H_