#pragma once
#ifndef NYAA_SYSTEM_WORLD_GENERATING_SYSTEM_H_
#define NYAA_SYSTEM_WORLD_GENERATING_SYSTEM_H_

#include "system/system.h"
#include "game/world-def.h"
#include "game/vector.h"

namespace nyaa {

namespace com {
class ZoneComponent;
}  // namespace com

class WorldMap;
class PseudoRandomGenerator;

namespace sys {

class WorldGeneratingSystem final : public base::ArenaObject {
public:
    WorldGeneratingSystem() = default;

    std::string InitWorldMap(std::string_view name, PseudoRandomGenerator *random, WorldMapDef::Size size,
                             std::string *err);

    Vector2i ReadWorldSize(std::string_view world_id, std::string *err);
    void     ReadWorldIndex(std::string_view world_id, WorldMap *index, std::string *err);

    void Generate(uint64_t seed, ZoneEnv::Kind env, com::ZoneComponent *zone);

    DISALLOW_IMPLICIT_CONSTRUCTORS(WorldGeneratingSystem);

private:
    class Core;
};  // class ZoneGeneratingSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_SYSTEM_WORLD_GENERATING_SYSTEM_H_