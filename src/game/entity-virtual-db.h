#pragma once
#ifndef NYAA_GAME_ENTITY_GRID_DB_H_
#define NYAA_GAME_ENTITY_GRID_DB_H_

#include "base/base.h"

namespace nyaa {

namespace entity {
class Entity;
}  // namespace entity

class EntityGrid;
class EntityGridSet;

class EntityVirtualDB {
public:
    inline EntityVirtualDB() = default;
    virtual ~EntityVirtualDB();
    
    static EntityVirtualDB *New(EntityGridSet *entity_grids);

    DISALLOW_IMPLICIT_CONSTRUCTORS(EntityVirtualDB);
};  // class EntitiesGridDB

}  // namespace nyaa

#endif  // NYAA_GAME_ENTITY_GRID_DB_H_