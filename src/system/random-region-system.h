#pragma once
#ifndef NYAA_SYSTEM_RANDOM_REGION_SYSTEM_H_
#define NYAA_SYSTEM_RANDOM_REGION_SYSTEM_H_

#include "base/base.h"

namespace nyaa {

namespace entity {
class Region;
}  // namespace entity

namespace sys {

class RandomRegionSystem {
public:
    RandomRegionSystem() = default;

    void FillRegion(entity::Region *region);

    DISALLOW_IMPLICIT_CONSTRUCTORS(RandomRegionSystem);

private:
};  // class RandomRegionSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_SYSTEM_RANDOM_REGION_SYSTEM_H_