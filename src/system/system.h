#pragma once
#ifndef NYAA_SYSTEM_SYSTEM_H_
#define NYAA_SYSTEM_SYSTEM_H_

#include "base/base.h"
#include <memory>

namespace nyaa {

namespace sys {

class EntityAllocationSystem;
class ZoneRenderSystem;
class RandomZoneSystem;
class ZoneLoadingSystem;
class ZoneGeneratingSystem;
class ActorMovementSystem;
class ActorAISystem;
class ActorBillboardRenderSystem;
class AvatarRenderSystem;
class GeometryTransformSystem;
class SpriteRenderSystem;

class System final {
public:
    System();
    ~System();

    DEF_VAL_GETTER(bool, initialized);

    void Prepare();

    sys::EntityAllocationSystem *    entity_allocator() const { return entity_allocator_.get(); }
    sys::ZoneRenderSystem *          zone_render() const { return zone_render_.get(); }
    sys::ZoneLoadingSystem *         zone_loader() const { return zone_loader_.get(); }
    sys::AvatarRenderSystem *        avatar_render() const { return avatar_render_.get(); }
    sys::SpriteRenderSystem *        sprite_render() const { return sprite_render_.get(); }
    sys::RandomZoneSystem *          random_zone() const { return random_zone_.get(); }
    sys::ActorBillboardRenderSystem *actor_billboard() const { return actor_billboard_.get(); }
    sys::ActorMovementSystem *       actor_movement() const { return actor_movement_.get(); }
    sys::ActorAISystem *             actor_ai() const { return actor_ai_.get(); }
    sys::GeometryTransformSystem *   transform() const { return transform_.get(); }

    static System *This();

    DISALLOW_IMPLICIT_CONSTRUCTORS(System);

private:
    // Systems
    std::unique_ptr<sys::EntityAllocationSystem>     entity_allocator_;
    std::unique_ptr<sys::ZoneRenderSystem>           zone_render_;
    std::unique_ptr<sys::ZoneLoadingSystem>          zone_loader_;
    std::unique_ptr<sys::RandomZoneSystem>           random_zone_;
    std::unique_ptr<sys::ActorMovementSystem>        actor_movement_;
    std::unique_ptr<sys::ActorAISystem>              actor_ai_;
    std::unique_ptr<sys::ActorBillboardRenderSystem> actor_billboard_;
    std::unique_ptr<sys::SpriteRenderSystem>         sprite_render_;
    std::unique_ptr<sys::AvatarRenderSystem>         avatar_render_;
    std::unique_ptr<sys::GeometryTransformSystem>    transform_;

    bool initialized_ = false;
};  // class System

}  // namespace sys

using sys::System;

}  // namespace nyaa

#endif  // NYAA_SYSTEM_SYSTEM_H_