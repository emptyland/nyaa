#pragma once
#ifndef NYAA_GAME_WORLD_DEF_H_
#define NYAA_GAME_WORLD_DEF_H_

#include "game/vector.h"
#include "base/base.h"
#include "glog/logging.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace nyaa {

struct WorldMapDef {
    enum Size {
        kSmall,
        kNormal,
        kLarge,
        kMaxKinds,
    };

    static inline Vector2i SizeOf(Size kind);
};  // struct WorldMapDef

extern const Vector2i kMapSizeDef[WorldMapDef::kMaxKinds];

struct ZoneEnv {
    enum Kind {
        kPlain,
        kForest,
        kSnowfield,
        kMaxKinds,
    };

    static inline const ZoneEnv *Of(Kind kind);

    static const ZoneEnv *Choose(float n);

    Kind        env;
    const char *name;
    float       ratio;
};  // struct ZoneEnv

extern const ZoneEnv kAllZoneEnv[ZoneEnv::kMaxKinds];

struct Files {
    static constexpr uint32_t kVersion = 0x1;

    static constexpr char kWorldListName[]  = "world-list.txt";
    static constexpr char kWorldsDirName[]  = "worlds";
    static constexpr char kWorldIndexName[] = "world-index.dat";
    static constexpr char kWorldIndexMagic[] = "world:";
    static constexpr int kWorldIdSize = 8;
    static constexpr int kWorldIndexHeaderSize = sizeof(kWorldIndexMagic) + kWorldIdSize;

    static fs::path WorldsPath();
    static fs::path WorldPath(std::string_view world_id) { return WorldsPath().append(world_id); }
    static fs::path WorldIndexPath(std::string_view world_id) { return WorldPath(world_id).append(kWorldIndexName); }

    DISALLOW_ALL_CONSTRUCTORS(Files);
};  // struct Files

/*static*/ inline const ZoneEnv *ZoneEnv::Of(Kind kind) {
    int index = static_cast<int>(kind);
    DCHECK_GE(index, 0);
    DCHECK_LT(index, kMaxKinds);
    return &kAllZoneEnv[index];
}

/*static*/ inline Vector2i WorldMapDef::SizeOf(Size kind) {
    int index = static_cast<int>(kind);
    DCHECK_GE(index, 0);
    DCHECK_LT(index, kMaxKinds);
    return kMapSizeDef[index];
}

}  // namespace nyaa

#endif  // NYAA_GAME_WORLD_DEF_H_