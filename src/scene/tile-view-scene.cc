#include "scene/tile-view-scene.h"
#include "game/game.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

TileViewScene::TileViewScene(Game *game): Scene(game) {}
TileViewScene::~TileViewScene() {}

void TileViewScene::Reset() {}
void TileViewScene::OnKeyInput(int key, int code, int action, int mods) {}
void TileViewScene::Render(double delta) {}

}  // namespace nyaa
