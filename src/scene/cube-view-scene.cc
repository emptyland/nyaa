#include "scene/cube-view-scene.h"
#include "system/geometry-transform-system.h"
#include "resource/texture-library.h"
#include "resource/font-library.h"
#include "game/game.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

class CubeViewScene::Core {
public:
    Core(CubeViewScene *owns) : owns_(owns) {}

    Game *game() const { return owns_->game(); }

private:
    CubeViewScene *const owns_;
};  // class CubeViewScene::Core

CubeViewScene::CubeViewScene(Game *game) : Scene(game), core_(new Core(this)) {}
CubeViewScene::~CubeViewScene() {}

void CubeViewScene::Reset() {}
void CubeViewScene::OnKeyInput(int key, int code, int action, int mods) {}
void CubeViewScene::Render(double delta) {}

}  // namespace nyaa
