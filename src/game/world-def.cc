#include "game/world-def.h"
#include "game/properties.h"
#include "game/game.h"
#include "game/vector.h"

namespace nyaa {

const Vector2i kMapSizeDef[WorldMapDef::kMaxKinds] = {
    // :format
    {32, 32},
    {64, 64},
    {128, 128},
};

const ZoneEnv kAllZoneEnv[ZoneEnv::kMaxKinds] = {
    {
        ZoneEnv::kPlain,
        "Plain",
        0.2f,
    },
    {
        ZoneEnv::kForest,
        "Forest",
        0.2f,
    },
    {
        ZoneEnv::kSnowfield,
        "Snowfield",
        0.1f,
    },
};

/*static*/ const ZoneEnv *ZoneEnv::Choose(float n) {
    DCHECK_GE(n, 0);
    DCHECK_LE(n, 1);

    float gradient[kMaxKinds];
    gradient[0] = kAllZoneEnv[0].ratio;
    for (int i = 1; i < kMaxKinds; i++) { gradient[i] = gradient[i - 1] + kAllZoneEnv[i].ratio; }

    for (int i = 0; i < kMaxKinds; i++) {
        if (n < gradient[i]) { return &kAllZoneEnv[i]; }
    }
    return &kAllZoneEnv[0];
}

/*static*/ fs::path Files::WorldsPath() {
    fs::path path;
    return path.append(Game::This()->properties()->data_dir()).append(kWorldsDirName);
}

/*static*/ fs::path Files::WorldPath(std::string_view world_id) { return WorldsPath().append(world_id); }

}  // namespace nyaa
