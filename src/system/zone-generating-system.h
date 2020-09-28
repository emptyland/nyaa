#pragma once
#ifndef NYAA_SYSTEM_ZONE_GENERATING_SYSTEM_H_
#define NYAA_SYSTEM_ZONE_GENERATING_SYSTEM_H_

#include "system/system.h"
#include "game/vector.h"

namespace nyaa {

namespace com {
class ZoneComponent;
}  // namespace com

namespace sys {

class ZoneGeneratingSystem final {
public:
    ZoneGeneratingSystem() = default;

    void Generate(com::ZoneComponent *zone);

    DISALLOW_IMPLICIT_CONSTRUCTORS(ZoneGeneratingSystem);
};  // class ZoneGeneratingSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_SYSTEM_ZONE_GENERATING_SYSTEM_H_