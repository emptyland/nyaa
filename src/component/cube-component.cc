#include "component/cube-component.h"
#include "game/game.h"

namespace nyaa {

namespace com {

res::Texture *CubeComponent::GetTopTexture() const { return Game::This()->cube_lib()->cube(kind())->top_tex(); }

res::Texture *CubeComponent::GetEdgeTexture() const { return Game::This()->cube_lib()->cube(kind())->edge_tex(); }

}  // namespace com

}  // namespace nyaa
