#pragma once
#ifndef NYAA_GAME_WORLD_MAP_H_
#define NYAA_GAME_WORLD_MAP_H_

#include "game/world-def.h"
#include "base/arena.h"
#include "glog/logging.h"

namespace nyaa {
namespace sys {
class WorldGeneratingSystem;
}  // namespace sys

class WorldMap final {
public:
    struct Index {
        Vector2i      coord;
        uint64_t      seed;
        ZoneEnv::Kind env;
    };  // struct Index

    DEF_VAL_GETTER(uint32_t, version);
    DEF_VAL_GETTER(Vector2i, size);

    const Index *index(int x, int y) const {
        DCHECK_GE(x * y, 0);
        DCHECK_LT(x * y, size_.x * size_.y);
        return &indices_[y * size_.x + x];
    }

    const char *world_id() const { return world_id_; }

    static size_t RequiredSize(Vector2i size) { return sizeof(WorldMap) + sizeof(Index) * size.x * size.y; }

    static WorldMap *New(base::Arena *arena, std::string_view id, Vector2i size) {
        const size_t request_size = RequiredSize(size);
        void *       chunk        = arena->Allocate(request_size);
        return new (chunk) WorldMap(id, size);
    }

    friend class sys::WorldGeneratingSystem;
    DISALLOW_IMPLICIT_CONSTRUCTORS(WorldMap);

private:
    WorldMap(std::string_view id, Vector2i size) : size_(size) {
        DCHECK_LT(id.size(), arraysize(world_id_));
        ::memcpy(world_id_, id.data(), id.size());
        world_id_[id.size()] = '\0';
        ::memset(indices_, 0, sizeof(Index) * size.x * size.y);
    }

    Index *get(int x, int y) {
        DCHECK_GE(x * y, 0);
        DCHECK_LT(x * y, size_.x * size_.y);
        return &indices_[y * size_.x + x];
    }

    Vector2i size_;
    uint32_t version_ = 0;
    char     world_id_[10];
    Index    indices_[0];
};  // class WorldMapIndex

}  // namespace nyaa

#endif  // NYAA_GAME_WORLD_MAP_H_