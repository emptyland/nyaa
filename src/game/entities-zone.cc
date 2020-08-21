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

EntitiesRegion::~EntitiesRegion() {

}

EntitiesZone::EntitiesZone() {
    ::memset(sibling_, 0, sizeof(sibling_));
    for (int i = 0; i < arraysize(sibling_); i++) {
        free_regions_.push_back(new EntitiesRegion());
    }
}

EntitiesZone::~EntitiesZone() {
    for (EntitiesRegion *region : free_regions_) {
        delete region;
    }
    delete region_;
    for (int i = 0; i < arraysize(sibling_); i++) {
        delete sibling_[i];
    }
}

void EntitiesZone::ScrollEastToWest(Vertex2i coord) {
    EntitiesRegion *east = region_;
    region_ = sibling_[0];

    sibling_[0] = east;
}

void EntitiesZone::ScrollWestToEast(Vertex2i coord) {

}

void EntitiesZone::ScrollNorthToSouth(Vertex2i coord) {

}

void EntitiesZone::ScrollSouthToNorth(Vertex2i coord) {

}

}  // namespace nyaa
