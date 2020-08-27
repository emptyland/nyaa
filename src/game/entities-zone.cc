#include "game/entities-zone.h"

namespace nyaa {

EntitiesRegion::EntitiesRegion() {
    for (int j = 0; j < kRegionSize; j++) {
        for (int i = 0; i < kRegionSize; i++) {
            grids_[i][j].local_coord_.x = i;
            grids_[i][j].local_coord_.y = j;
        }
    }
}

EntitiesRegion::~EntitiesRegion() {}

}  // namespace nyaa
