#pragma once
#ifndef NYAA_SYSTEM_SPRITE_RENDER_SYSTEM_H_
#define NYAA_SYSTEM_SPRITE_RENDER_SYSTEM_H_

#include "system/system.h"
#include "game/vector.h"

namespace nyaa {
namespace com {
class PlantComponent;
}  // namespace com

namespace res {
class SpriteLibrary;
class Sprite;
}  // namespace res

namespace sys {

class SpriteRenderSystem final : public base::ArenaObject {
public:
    SpriteRenderSystem() = default;

    void Prepare(res::SpriteLibrary *lib);

    void RenderPlant(const Vector3f &view, com::PlantComponent *plant, double delta);

    void Render(res::Sprite *def, int frame_index, double delta);

private:
    uint32_t vbo_         = 0;
    bool     initialized_ = false;
};  // class SpriteRenderSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_SYSTEM_SPRITE_RENDER_SYSTEM_H_