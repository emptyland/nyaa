#include "scene/tile-view-scene.h"
#include "system/geometry-transform-system.h"
#include "resource/texture-library.h"
#include "resource/font-library.h"
#include "game/game.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

class TileViewScene::Core {
public:
    static constexpr int kTileW = 64;

    Core(TileViewScene *owns) : owns_(owns) {}

    void DrawPage(Game *game, double delta) {

    }

    void UpdatePageSize() {

    }

    int PageSize() const { return page_col_ * page_row_; }
private:
    TileViewScene *const owns_;
    int page_col_ = 0;
    int page_row_ = 0;
    int page_no_ = 0;
};  // class TileViewScene::Core

TileViewScene::TileViewScene(Game *game) : Scene(game), core_(new Core(this)) {}
TileViewScene::~TileViewScene() {}

void TileViewScene::Reset() {}
void TileViewScene::OnKeyInput(int key, int code, int action, int mods) {}

void TileViewScene::Render(double delta) {
    game()->transform()->Enter2DProjection();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ctrl_->DrawPage(game(), delta);

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

    game()->transform()->Exit2DProjection();
}

}  // namespace nyaa
