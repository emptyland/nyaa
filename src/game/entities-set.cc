#include "game/entities-set.h"
#include "entity/plant-entity.h"
#include "entity/player-entity.h"
#include "entity/actor-entity.h"
#include "component/zone-component.h"

namespace nyaa {

void EntitiesGrid::Enter(entity::Entity *obj) {
    if (obj->grid() == this) { return; }
    entity::Entity *x = dummy()->next();
    while (x != dummy()) {
        if (obj->ZOrder() < x->ZOrder()) { break; }
        x = x->next();
    }
    QUEUE_INSERT_TAIL(x, obj);
    obj->set_grid(this);
}

EntitiesSet::EntitiesSet() {
    for (int j = 0; j < kRegionSize; j++) {
        for (int i = 0; i < kRegionSize; i++) {
            grids_[i][j].coord_.x = i;
            grids_[i][j].coord_.y = j;
        }
    }
}

EntitiesSet::~EntitiesSet() {}

EntitiesGrid *EntitiesSet::ViewGrid(const com::ViewportComponent &viewport, int x, int y) {
    int abs_x = viewport.round_down_center_x() - viewport.bound().x / 2 + x;
    int abs_y = viewport.round_down_center_y() - viewport.bound().y / 2 + y;

    if (abs_x < 0 || abs_x >= kRegionSize) { return &empty_grid_; }
    if (abs_y < 0 || abs_y >= kRegionSize) { return &empty_grid_; }
    return &grids_[abs_x][abs_y];
}

void EntitiesSet::UpdatePlayer(entity::PlayerEntity *obj) { Update(obj->movement().coord(), obj); }

void EntitiesSet::UpdateActor(entity::ActorEntity *obj) { Update(obj->movement().coord(), obj); }

void EntitiesSet::UpdatePlant(entity::PlantEntity *obj) { Update(obj->plant()->position(), obj); }

}  // namespace nyaa
