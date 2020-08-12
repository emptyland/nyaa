#pragma once
#ifndef NYAA_ENTITY_REGION_H_
#define NYAA_ENTITY_REGION_H_

#include "component/cube-component.h"
#include "entity/entity.h"
#include "game/constants.h"

namespace nyaa {

namespace entity {

class Region : public Entity {
public:
    using Floor = com::FloorComponent<kRegionSize, kRegionSize>;

    Region() : Entity(EntityId::Of(0)) {}

    Floor *floor(int i) {
        DCHECK_GE(i, 0);
        DCHECK_LT(i, kTerrainMaxLevels);
        return &terrain_[i];
    }

private:
    Floor terrain_[kTerrainMaxLevels];
};  // class Region

}  // namespace entity

}  // namespace nyaa

#endif  // NYAA_ENTITY_REGION_H_