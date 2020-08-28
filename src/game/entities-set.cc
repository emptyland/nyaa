#include "game/entities-set.h"
#include "entity/plant-entity.h"
#include "entity/player-entity.h"

namespace nyaa {

void EntitiesGrid::Enter(entity::Entity *obj) {
    if (obj->grid() == this) { return; }
    entity::Entity *x = dummy()->next();
    while (x != dummy()) {
        if (obj->ZOrder() < x->ZOrder()) { break; }
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

void EntitiesSet::UpdatePlayer(entity::PlayerEntity *obj) { Update(obj->movement().coord(), obj); }

void EntitiesSet::UpdatePlant(entity::PlantEntity *obj) { Update(obj->plant()->position(), obj); }

}  // namespace nyaa
