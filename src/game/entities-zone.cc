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

EntitiesZone::EntitiesZone() {
    ::memset(sibling_, 0, sizeof(sibling_));
    for (int i = 0; i < arraysize(sibling_); i++) { free_regions_.push_back(new EntitiesRegion()); }
}

EntitiesZone::~EntitiesZone() {
    for (EntitiesRegion *region : free_regions_) { delete region; }
    delete region_;
    for (int i = 0; i < arraysize(sibling_); i++) { delete sibling_[i]; }
}

void EntitiesZone::ScrollEastToWest(Vector2i coord) {
    EntitiesRegion *east = region_;
    region_              = sibling_[0];
    ReAllocateIfNeeded(-1, {coord.x + kRegionSize, coord.y});

    sibling_[0] = east;
    ReAllocateIfNeeded(0, coord);

    FreeIfNeeded(1);
    FreeIfNeeded(2);
}

void EntitiesZone::ScrollWestToEast(Vector2i coord) {
    EntitiesRegion *west = region_;
    region_              = sibling_[0];
    ReAllocateIfNeeded(-1, {coord.x - kRegionSize, coord.y});

    sibling_[0] = west;
    ReAllocateIfNeeded(0, coord);

    FreeIfNeeded(1);
    FreeIfNeeded(2);
}

void EntitiesZone::ScrollNorthToSouth(Vector2i coord) {
    EntitiesRegion *north = region_;
    region_               = sibling_[0];
    ReAllocateIfNeeded(-1, {coord.x, coord.y + kRegionSize});

    sibling_[0] = north;
    ReAllocateIfNeeded(0, coord);

    FreeIfNeeded(1);
    FreeIfNeeded(2);
}

void EntitiesZone::ScrollSouthToNorth(Vector2i coord) {
    EntitiesRegion *south = region_;

    region_ = sibling_[0];
    ReAllocateIfNeeded(-1, {coord.x, coord.y - kRegionSize});

    sibling_[0] = south;
    ReAllocateIfNeeded(0, coord);

    FreeIfNeeded(1);
    FreeIfNeeded(2);
}

void EntitiesZone::ScrollSEToSW(Vector2i coord) {
    //    [SE->0] | [0->SW]
    // -----------+---------
    //        [2] | [1]
    EntitiesRegion *se = region_;

    region_ = sibling_[0];
    ReAllocateIfNeeded(-1, {coord.x + kRegionSize, coord.y});

    sibling_[0] = se;
    ReAllocateIfNeeded(0, coord);

    ReAllocateIfNeeded(1, {coord.x + kRegionSize, coord.y + kRegionSize});

    ReAllocateIfNeeded(2, {coord.x, coord.y + kRegionSize});
}

void EntitiesZone::ScrollSEToNE(Vector2i coord) {
    //    [SE->0]  | [0->1]
    // ------------+------
    //    [2->NE]  | [1->2]
    EntitiesRegion *se = region_;

    region_ = sibling_[2];
    ReAllocateIfNeeded(-1, {coord.x, coord.y + kRegionSize});

    sibling_[2] = sibling_[1];
    ReAllocateIfNeeded(2, {coord.x + kRegionSize, coord.y + kRegionSize});

    sibling_[1] = sibling_[0];
    ReAllocateIfNeeded(1, {coord.x + kRegionSize, coord.y});

    sibling_[0] = se;
    ReAllocateIfNeeded(0, {coord.x, coord.y});
}

void EntitiesZone::ScrollSEToNW(Vector2i coord) {
    //    [SE->0] | [0->1]
    // -----------+---------
    //        [2] | [1->NW]
    EntitiesRegion *se = region_;

    region_ = sibling_[1];
    ReAllocateIfNeeded(-1, {coord.x + kRegionSize, coord.y + kRegionSize});

    ReAllocateIfNeeded(2, {coord.x, coord.y + kRegionSize});

    sibling_[1] = sibling_[0];
    ReAllocateIfNeeded(1, {coord.x + kRegionSize, coord.y});

    sibling_[0] = se;
    ReAllocateIfNeeded(0, coord);
}

void EntitiesZone::ScrollNEToSE(Vector2i coord) {
    //     [0->SE] | [1->0]
    // ------------+--------
    //     [NE->2] | [2->1]
    EntitiesRegion *ne = region_;

    region_ = sibling_[0];
    ReAllocateIfNeeded(1, {coord.x, coord.y - kRegionSize});

    sibling_[0] = sibling_[1];
    ReAllocateIfNeeded(1, {coord.x + kRegionSize, coord.y - kRegionSize});

    sibling_[1] = sibling_[2];
    ReAllocateIfNeeded(1, {coord.x + kRegionSize, coord.y});

    sibling_[2] = ne;
    ReAllocateIfNeeded(1, coord);
}

void EntitiesZone::ScrollNEToNW(Vector2i coord) {
    //        [0] | [1]
    // -----------+---------
    //    [NE->2] | [2->NW]
    EntitiesRegion *ne = region_;

    region_ = sibling_[2];
    ReAllocateIfNeeded(-1, {coord.x + kRegionSize, coord.y});

    sibling_[2] = ne;
    ReAllocateIfNeeded(2, coord);

    ReAllocateIfNeeded(0, {coord.x, coord.y - kRegionSize});

    ReAllocateIfNeeded(1, {coord.x + kRegionSize, coord.y - kRegionSize});
}

void EntitiesZone::ScrollNEToSW(Vector2i coord) {
    //        [0] | [1->SW]
    // -----------+---------
    //    [NE->2] | [2->1]
    EntitiesRegion *ne = region_;

    region_ = sibling_[1];
    ReAllocateIfNeeded(-1, {coord.x + kRegionSize, coord.y - kRegionSize});

    sibling_[1] = sibling_[2];
    ReAllocateIfNeeded(1, {coord.x + kRegionSize, coord.y});

    sibling_[2] = ne;
    ReAllocateIfNeeded(2, coord);

    ReAllocateIfNeeded(0, {coord.x, coord.y - kRegionSize});
}

void EntitiesZone::ScrollNWToSW(Vector2i coord) {
    //   [0] | [1->SW]
    // ------+---------
    //   [2] | [NW->1]
    EntitiesRegion *nw = region_;

    region_ = sibling_[1];
    ReAllocateIfNeeded(-1, {coord.x, coord.y - kRegionSize});

    sibling_[1] = nw;
    ReAllocateIfNeeded(1, coord);

    ReAllocateIfNeeded(0, {coord.x - kRegionSize, coord.y - kRegionSize});

    ReAllocateIfNeeded(2, {coord.x - kRegionSize, coord.y});
}

void EntitiesZone::ScrollNWToNE(Vector2i coord) {
    //       [0] | [1]
    // ----------+------
    //   [2->NE] | [NW->2]
    EntitiesRegion *nw = region_;

    region_ = sibling_[2];
    ReAllocateIfNeeded(-1, {coord.x - kRegionSize, coord.y});

    sibling_[2] = nw;
    ReAllocateIfNeeded(2, coord);

    ReAllocateIfNeeded(0, {coord.x - kRegionSize, coord.y - kRegionSize});

    ReAllocateIfNeeded(1, {coord.x, coord.y - kRegionSize});
}

void EntitiesZone::ScrollNWToSE(Vector2i coord) {
    //   [0->SE] | [1->0]
    // ----------+---------
    //       [2] | [NW->1]
    EntitiesRegion *nw = region_;

    region_ = sibling_[0];
    ReAllocateIfNeeded(-1, {coord.x - kRegionSize, coord.y - kRegionSize});

    sibling_[0] = sibling_[1];
    ReAllocateIfNeeded(0, {coord.x, coord.y - kRegionSize});

    sibling_[1] = nw;
    ReAllocateIfNeeded(1, coord);

    ReAllocateIfNeeded(2, {coord.x - kRegionSize, coord.y});
}

}  // namespace nyaa
