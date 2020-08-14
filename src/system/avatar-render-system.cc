#include "system/avatar-render-system.h"
#include "component/avatar-component.h"
#include "component/movement-component.h"

namespace nyaa {
namespace sys {

void AvatarRenderSystem::Render(const com::MoventComponent &move, com::AvatarComponent *avatar, double time_delta) {
    avatar->AddTime(time_delta);
    // TODO:
}

}  // namespace sys

}  // namespace nyaa