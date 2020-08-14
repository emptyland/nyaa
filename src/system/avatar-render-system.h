#pragma once
#ifndef NYAA_SYSTEM_AVATAR_RENDER_SYSTEM_H_
#define NYAA_SYSTEM_AVATAR_RENDER_SYSTEM_H_

#include "base/base.h"

namespace nyaa {
namespace com {
class AvatarComponent;
class MoventComponent;
} // namespace com

namespace sys {

class AvatarRenderSystem {
public:
    void Render(const com::MoventComponent &move, com::AvatarComponent *avatar, double time_delta);

    DISALLOW_IMPLICIT_CONSTRUCTORS(AvatarRenderSystem);
}; // class AvatarRenderSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_SYSTEM_AVATAR_RENDER_SYSTEM_H_