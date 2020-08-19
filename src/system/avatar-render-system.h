#pragma once
#ifndef NYAA_SYSTEM_AVATAR_RENDER_SYSTEM_H_
#define NYAA_SYSTEM_AVATAR_RENDER_SYSTEM_H_

#include "base/base.h"

namespace nyaa {
namespace com {
class AvatarComponent;
class MovementComponent;
} // namespace com

namespace sys {

class AvatarRenderSystem {
public:
    AvatarRenderSystem() = default;

    void Render(com::MovementComponent *move, com::AvatarComponent *avatar, double delta);

    DISALLOW_IMPLICIT_CONSTRUCTORS(AvatarRenderSystem);
}; // class AvatarRenderSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_SYSTEM_AVATAR_RENDER_SYSTEM_H_