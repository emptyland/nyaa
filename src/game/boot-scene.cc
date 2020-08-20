#include "game/boot-scene.h"
#include "game/test-scene.h"
#include "game/identifiers.h"
#include "game/game.h"
#include "component/avatar-component.h"
#include "component/cube-component.h"
#include "system/geometry-transform-system.h"
#include "resource/texture-library.h"
#include "resource/font-library.h"
#include "resource/avatar-library.h"
#include <GLFW/glfw3.h>

namespace nyaa {

BootScene::BootScene(Game *game) : Scene(game) {}

BootScene::~BootScene() {}

void BootScene::Reset() {
    res::Avatar *def = game()->avatar_lib()->FindOrNull(ResourceId::Of(100000));
    DCHECK(def != nullptr);
    avatar1_.reset(new com::AvatarComponent(def));
    avatar1_->set_speed(0.9);

    def = game()->avatar_lib()->FindOrNull(ResourceId::Of(100050));
    DCHECK(def != nullptr);
    avatar2_.reset(new com::AvatarComponent(def));
    avatar2_->set_speed(0.9);
}

void BootScene::OnKeyInput(int key, int code, int action, int mods) {
    switch (key) {
        case GLFW_KEY_ESCAPE: game()->Exit(); break;

        case GLFW_KEY_T: {
            TestScene *test_scene = new TestScene(game());
            test_scene->SwitchTo(this);
        } break;

        default: break;
    }
}

void BootScene::OnMouseInput(double x, double y) {}

void BootScene::Render(double d) {
    game()->font_lib()->default_face()->Render(res::TEST_STRING_1, 0, game()->fb_h() / 2 + 50);
    // game()->font_lib()->default_face()->Render(res::TEST_STRING_2, 0, game()->fb_h()/2, {0,1,0});

    game()->transform()->Enter2DProjection();
    // const res::Texture *tex = game()->texture_lib()->FindOrNull(ResourceId::Of(102080));
    // DCHECK(tex != nullptr);
    avatar1_->AddTime(d);
    avatar2_->AddTime(d);
    res::Texture *tex = avatar1_->GetFrame();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, tex->tex_id());
    glBegin(GL_QUADS);

    const int w = 24 * 3;
    const int h = 32 * 3;

    // 0,1(3) 1,1(2)
    // 0,0(0) 1,0(1)
    glTexCoord2f(tex->coord(0).x, tex->coord(0).y);
    glVertex2i(0, h);

    glTexCoord2f(tex->coord(1).x, tex->coord(1).y);
    glVertex2i(w, h);

    glTexCoord2f(tex->coord(2).x, tex->coord(2).y);
    glVertex2i(w, 0);

    glTexCoord2f(tex->coord(3).x, tex->coord(3).y);
    glVertex2i(0, 0);

    tex = avatar2_->GetFrame();
    glTexCoord2f(tex->coord(0).x, tex->coord(0).y);
    glVertex2i(w, h);

    glTexCoord2f(tex->coord(1).x, tex->coord(1).y);
    glVertex2i(w + w, h);

    glTexCoord2f(tex->coord(2).x, tex->coord(2).y);
    glVertex2i(w + w, 0);

    glTexCoord2f(tex->coord(3).x, tex->coord(3).y);
    glVertex2i(w, 0);

    glEnd();

    game()->transform()->Exit2DProjection();
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

}  // namespace nyaa
