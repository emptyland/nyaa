#include "game/world.h"
#include "game/world-def.h"
#include "resource/texture-library.h"
#include "resource/font-library.h"
#include "resource/shader-library.h"
#include "entity/player-entity.h"
#include "entity/plant-entity.h"
#include "entity/actor-entity.h"
#include "component/zone-component.h"
#include "component/cube-component.h"
#include "component/avatar-component.h"
#include "system/entity-allocation-system.h"
#include "system/geometry-transform-system.h"
#include "system/random-zone-system.h"
#include "system/zone-render-system.h"
#include "system/zone-loading-system.h"
#include "system/actor-movement-system.h"
#include "system/avatar-render-system.h"
#include "system/sprite-render-system.h"
#include "system/actor-billboard-render-system.h"
#include "system/actor-ai-system.h"
#include "system/impact-checking-system.h"
#include "game/game.h"
#include "game/matrix.h"
#include "game/entity-grid-set.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

World::World() : random_(0) {
    // :format
}

World::~World() {
    // :format
}

bool World::Prepare() { return false; }

}  // namespace nyaa
