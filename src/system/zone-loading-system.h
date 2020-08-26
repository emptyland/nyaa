#pragma once
#ifndef NYAA_SYSETM_ZONE_LOADING_SYSTEM_H_
#define NYAA_SYSETM_ZONE_LOADING_SYSTEM_H_

#include "component/zone-component.h"
#include "base/base.h"
#include <unordered_map>

namespace nyaa {
namespace com {
class ZoneComponent;
class RegionComponent;
}  // namespace com

namespace sys {

class ZoneLoadingListener {
public:
    virtual void DidLoadRegion(int index, com::RegionComponent *region) = 0;
    virtual void DidFreeRegion(int index, com::RegionComponent *region) = 0;
    virtual void OnScrollRegion(com::RegionComponent *origin, com::ZoneComponent::Direction from,
                                com::ZoneComponent::Direction to)       = 0;

    DISALLOW_IMPLICIT_CONSTRUCTORS(ZoneLoadingListener);
};  // class ZoneLoadingListener

class ZoneLoadingSystem {
public:
    ZoneLoadingSystem() = default;

    DEF_PTR_PROP_RW(ZoneLoadingListener, listener);

    void Update(com::ZoneComponent *zone);

    DISALLOW_IMPLICIT_CONSTRUCTORS(ZoneLoadingSystem);

private:
    void ReplaceRegionIfNeeded(com::ZoneComponent *zone, com::RegionComponent *center);
    com::RegionComponent *LoadRegion(int x, int y);

    ZoneLoadingListener *listener_ = nullptr;

    std::unordered_map<int, com::RegionComponent *> cached_regions_;
};  // class ZoneLoadingSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_SYSETM_ZONE_LOADING_SYSTEM_H_