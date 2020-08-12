#pragma once
#ifndef NYAA_ENTITY_REGION_ENTITY_H_
#define NYAA_ENTITY_REGION_ENTITY_H_

#include "component/cube-component.h"
#include "entity/entity.h"
#include "game/constants.h"

namespace nyaa {

namespace entity {

class RegionEntity {
public:
    using Floor = com::FloorComponent<kRegionSize,kRegionSize>;

    Floor *floor(int i) {
        DCHECK_GE(i, 0);
        DCHECK_LT(i, kRegionLevels);
        return &terrain_[i];
    }
private:
    Floor terrain_[kRegionLevels];
}; // class RegionEntity

}  // namespace entity

}  // namespace nyaa

#endif  // NYAA_ENTITY_REGION_ENTITY_H_