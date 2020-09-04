#pragma once
#ifndef NYAA_ENTITY_PLANT_ENTITY_H_
#define NYAA_ENTITY_PLANT_ENTITY_H_

#include "component/plant-component.h"
#include "entity/entity.h"

namespace nyaa {

namespace entity {

class PlantEntity : public Entity {
public:
    PlantEntity(com::PlantComponent *plant) : Entity(EntityId::Of(0), Class<PlantEntity>::id()), plant_(plant) {}
    ~PlantEntity() final = default;

    DEF_PTR_GETTER(com::PlantComponent, plant);

    float ZOrder() const final { return -plant()->position().y; }

    float RadiusOrVolume() const final { return plant()->entity_size(); }

    DISALLOW_IMPLICIT_CONSTRUCTORS(PlantEntity);

private:
    com::PlantComponent *plant_;
};  // class PlantEntity

}  // namespace entity

}  // namespace nyaa

#endif  // NYAA_ENTITY_PLANT_ENTITY_H_