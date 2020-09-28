#pragma once
#ifndef NYAA_SYSETM_ZONE_LOADING_SYSTEM_H_
#define NYAA_SYSETM_ZONE_LOADING_SYSTEM_H_

#include "system/system.h"
#include "component/zone-component.h"
#include <unordered_map>

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
    void ReplaceRegionIfNeeded(com::ZoneComponent *zone, com::RegionComponent *center);
    com::RegionComponent *LoadRegion(int x, int y);

    std::unordered_map<int, com::RegionComponent *> cached_regions_;
};  // class ZoneLoadingSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_SYSETM_ZONE_LOADING_SYSTEM_H_