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

class PseudoRandomGenerator;

namespace sys {

class WorldGeneratingSystem final : public base::ArenaObject {
public:
    WorldGeneratingSystem() = default;

    bool InitWorldMap(PseudoRandomGenerator *random, WorldMapDef::Size size, std::string *err);

    void Generate(uint64_t seed, ZoneEnv::Kind env, com::ZoneComponent *zone);

    DISALLOW_IMPLICIT_CONSTRUCTORS(WorldGeneratingSystem);

private:
    class Core;
};  // class ZoneGeneratingSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_SYSTEM_WORLD_GENERATING_SYSTEM_H_