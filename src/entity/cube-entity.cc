#include "entity/cube-entity.h"
#include "game/game.h"
#include "resource/cube-library.h"
#include "resource/texture-library.h"

namespace nyaa {

namespace entity {

res::Texture *CubeEntity::GetTopTexture() const { return ThisGame->cube_lib()->cube(kind())->top_tex(); }

res::Texture *CubeEntity::GetEdgeTexture() const { return ThisGame->cube_lib()->cube(kind())->edge_tex(); }

}  // namespace entity

}  // namespace nyaa
