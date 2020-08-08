#include "game/boot-scene.h"
#include "game/identifiers.h"
#include "game/game.h"
#include "resource/font-library.h"
#include <GLFW/glfw3.h>

namespace nyaa {

BootScene::BootScene(Game *game)
    : Scene(game) {
}

BootScene::~BootScene() {
}

void BootScene::Reset() {

}

void BootScene::OnKeyInput(int key, int code, int action, int mods) {
    switch (key) {
    case GLFW_KEY_ESCAPE:
        game()->Exit();
        break;
    
    default:
        break;
    }
}

void BootScene::OnMouseInput(double x, double y) {
}

void BootScene::Render(double d) {
    game()->font_lib()->default_face()->Render(res::TEST_STRING_1, 0, game()->fb_h()/2 + 35);
    game()->font_lib()->default_face()->Render(res::TEST_STRING_2, 0, game()->fb_h()/2, {0,1,0});
}
    
} // namespace nyaa
