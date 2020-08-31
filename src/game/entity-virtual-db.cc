#include "game/entity-virtual-db.h"
#include "game/entity-grid-set.h"

namespace nyaa {

namespace {  // internal

class EntityVirtualDBImpl : public EntityVirtualDB {
public:
    EntityVirtualDBImpl(EntityGridSet *entity_grids) : entity_grids_(entity_grids) {}
    ~EntityVirtualDBImpl() final {}

private:
    EntityGridSet *entity_grids_;
};  // class EntityVirtualDBImpl

}  // namespace

/*virtual*/ EntityVirtualDB::~EntityVirtualDB() {}

/*static*/ EntityVirtualDB *EntityVirtualDB::New(EntityGridSet *entity_grids) {
    // new instance
    return new EntityVirtualDBImpl(entity_grids);
}

}  // namespace nyaa
