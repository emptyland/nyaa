#pragma once
#ifndef NYAA_SYSTEM_AVATAR_RENDER_SYSTEM_H_
#define NYAA_SYSTEM_AVATAR_RENDER_SYSTEM_H_

#include "system/system.h"
#include "game/vector.h"

namespace nyaa {
namespace com {
class AvatarComponent;
class MovementComponent;
}  // namespace com

namespace res {
class AvatarLibrary;
}  // namespace res

namespace sys {

class AvatarRenderSystem final : public base::ArenaObject {
public:
    AvatarRenderSystem() = default;

    void Prepare(res::AvatarLibrary *avatar_lib);

    void Render(com::MovementComponent *move, com::AvatarComponent *avatar, Vector3f *view, double delta);

    DISALLOW_IMPLICIT_CONSTRUCTORS(AvatarRenderSystem);

private:
    uint32_t vbo_         = 0;
    bool     initialized_ = false;

    static const float kVertices[];
};  // class AvatarRenderSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_SYSTEM_AVATAR_RENDER_SYSTEM_H_