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
    game()->font_lib()->default_face()->Render(res::TEST_STRING_1, 0, game()->fb_h()/2 + 50);
    game()->font_lib()->default_face()->Render(res::TEST_STRING_2, 0, game()->fb_h()/2, {0,1,0});

#if 0
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, ThisGame->fb_w(), 0, ThisGame->fb_h(), -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_CULL_FACE);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);

    glBindTexture(GL_TEXTURE_2D, game()->font_lib()->default_face()->buffered_tex());

    auto ch = game()->font_lib()->default_face()->FindOrInsertCharacter(62);

    glTexCoord2f(0, 0);
    glVertex2i(0, 0);

    glTexCoord2f(0, 1);
    glVertex2i(0, res::FontFace::kBufferTexH);

    glTexCoord2f(1, 1);
    glVertex2i(res::FontFace::kBufferTexW, res::FontFace::kBufferTexH);
    
    glTexCoord2f(1, 0);
    glVertex2i(res::FontFace::kBufferTexW, 0);

    glEnd();
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
#endif
}
    
} // namespace nyaa
