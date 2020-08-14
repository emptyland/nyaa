#pragma once
#ifndef NYAA_SYSTEM_RANDOM_REGION_SYSTEM_H_
#define NYAA_SYSTEM_RANDOM_REGION_SYSTEM_H_

#include "base/base.h"

namespace nyaa {

namespace com {
class RegionComponent;
}  // namespace entity

namespace sys {

class RandomRegionSystem {
public:
    RandomRegionSystem() = default;

    void Update(com::RegionComponent *region);

    DISALLOW_IMPLICIT_CONSTRUCTORS(RandomRegionSystem);

private:
};  // class RandomRegionSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_SYSTEM_RANDOM_REGION_SYSTEM_H_