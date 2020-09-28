#pragma once
#ifndef NYAA_SYSTEM_RANDOM_REGION_SYSTEM_H_
#define NYAA_SYSTEM_RANDOM_REGION_SYSTEM_H_

#include "system/system.h"

namespace nyaa {

namespace com {
class RegionComponent;
class ZoneComponent;
}  // namespace entity

namespace sys {

class RandomZoneSystem {
public:
    RandomZoneSystem() = default;

    void Update(com::ZoneComponent *zone);
    void Update(com::RegionComponent *region);

    DISALLOW_IMPLICIT_CONSTRUCTORS(RandomZoneSystem);

private:
};  // class RandomZoneSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_SYSTEM_RANDOM_REGION_SYSTEM_H_