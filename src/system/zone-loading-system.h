#pragma once
#ifndef NYAA_SYSETM_ZONE_LOADING_SYSTEM_H_
#define NYAA_SYSETM_ZONE_LOADING_SYSTEM_H_

#include "base/base.h"

namespace nyaa {
namespace com {
class ZoneComponent;
class RegionComponent;
}  // namespace com

namespace sys {

class ZoneLoadingSystem {
public:
    ZoneLoadingSystem() = default;

    void Update(com::ZoneComponent *zone);

    DISALLOW_IMPLICIT_CONSTRUCTORS(ZoneLoadingSystem);

private:
    void LoadRegionsIfNeeded(int x, int y);
    com::RegionComponent *LoadRegion(int x, int y);
};  // class ZoneLoadingSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_SYSETM_ZONE_LOADING_SYSTEM_H_