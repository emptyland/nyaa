#pragma once
#ifndef NYAA_SYSTEM_ACTOR_BILLBOARD_RENDER_SYSTEM_H_
#define NYAA_SYSTEM_ACTOR_BILLBOARD_RENDER_SYSTEM_H_

#include "game/identifiers.h"
#include "game/matrix.h"
#include "base/base.h"
#include <string_view>
#include <unordered_map>

namespace nyaa {
namespace com {
class NaturePropertiesComponent;
}  // namespace com

namespace sys {

class ActorBillboardRenderSystem {
public:
    ActorBillboardRenderSystem();

    DEF_VAL_PROP_RW(int, max_entries);

    void Render(const Vector3f &position, const Vector3f &color, EntityId id, com::NaturePropertiesComponent *nature,
                Vector3f *view);

    DISALLOW_IMPLICIT_CONSTRUCTORS(ActorBillboardRenderSystem);

private:
    struct VboEntry {
        VboEntry *  prev_;
        VboEntry *  next_;
        EntityId    id;
        uint32_t    vbo;
        int         count;
        Vector3f    center;
        Vector2f    size;
        std::string text;
    };

    VboEntry *EnsureBufferdVbo(const std::string &name, EntityId id);

    VboEntry *dummy() { return &dummy_; }

    VboEntry dummy_;
    int      max_entries_ = 1024;
    // uint32_t vbo_ = 0;
    // std::vector<float> buf_;
    std::unordered_map<EntityId, VboEntry *, EntityHash> entries_;
};  // class ActorBillboardRenderSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_SYSTEM_ACTOR_BILLBOARD_RENDER_SYSTEM_H_