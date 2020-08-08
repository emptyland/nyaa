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
    // for (int i = 0; i < 1000; i++) {
    //     EntityId id = game()->NewEntityId();
    //     printf("%08llx\n", id.value());
    // }
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
    //printf("mouse position: %f %f\n", x, y);
}

void BootScene::Render(double d) {
    game()->font_lib()->default_face()->Render("Hello 中文, World 汉字1234?", 0, game()->fb_h()/2);
    game()->font_lib()->default_face()->Render(res::TEST_STRING_1, 0, game()->fb_h()/2 + 32);
    //printf("render delta: %f\n", d);
}
    
} // namespace nyaa
