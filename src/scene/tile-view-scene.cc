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
    static constexpr int kBorderSize  = 48;
    static constexpr int kPaddingSize = 48;
    static constexpr int kTileW       = 64;
    static constexpr int kTileH       = 64;

    Core(TileViewScene *owns) : owns_(owns) {}

    DEF_VAL_GETTER(int, page_no);

    void DrawPage(double delta) {
        UpdatePageSize();

        res::FontFace *font = game()->font_lib()->system_face();

        int offset = page_no_ * PageSize();
        for (int j = 0; j < page_row_; j++) {
            for (int i = 0; i < page_col_; i++) {
                int index = offset++;
                if (index >= kTileSize) { return; }

                float x = i * (kTileW + kPaddingSize) + kBorderSize;
                float y = game()->fb_h() - (j + 1) * (kTileH + kPaddingSize) - kBorderSize;

                ResourceId id = ResourceId::Of(kTileIDStart + index * 10);
                DrawTile(x, y, game()->texture_lib()->FindOrNull(id), font);
            }
        }
    }

    void DrawTile(float x, float y, res::Texture *tex, res::FontFace *font) {
        char buf[64];
        ::snprintf(buf, arraysize(buf), "%d", tex->id().value());
        Vector2f size = font->ApproximateSize(buf) * font_scale_;

        Vector3f pos;
        pos.x = x + kTileW / 2;
        pos.x -= size.x / 2;
        pos.y = y - 32;
        pos.z = 0;

        std::vector<float> vertices;
        font->Render(pos, font_scale_, buf, &vertices);

        glBindTexture(GL_TEXTURE_2D, tex->tex_id());

        glBegin(GL_QUADS);
        glColor3f(1.0, 1.0, 1.0);
        glTexCoord2fv(&tex->coord(3).x);
        glVertex2f(x, y);

        glTexCoord2fv(&tex->coord(0).x);
        glVertex2f(x, y + kTileH);

        glTexCoord2fv(&tex->coord(1).x);
        glVertex2f(x + kTileW, y + kTileH);

        glTexCoord2fv(&tex->coord(2).x);
        glVertex2f(x + kTileW, y);

        glEnd();

        glBindTexture(GL_TEXTURE_2D, font->buffered_tex());

        glBegin(GL_QUADS);
        glColor3f(1.0, 1.0, 1.0);
        for (int i = 0; i < vertices.size(); i += 5) {
            glTexCoord2f(vertices[i + 3], vertices[i + 4]);
            glVertex3f(vertices[i + 0], vertices[i + 1], vertices[i + 2]);
        }
        glEnd();
    }

    void UpdatePageSize() {
        int w = game()->fb_w();
        int h = game()->fb_h();

        page_col_ = (w - 2 * kBorderSize) / (kTileW + kPaddingSize);
        page_row_ = (h - 2 * kBorderSize) / (kTileW + kPaddingSize);

        page_no_ = std::min(page_no_, MaxPageNo() - 1);
    }

    int PageSize() const { return page_col_ * page_row_; }

    int MaxPageNo() const { return (kTileSize + PageSize() - 1) / PageSize(); }

    void NextPage() { page_no_ = (page_no_ + 1) % MaxPageNo(); }

    void PrevPage() {
        if (page_no_ == 0) {
            page_no_ = MaxPageNo() - 1;
        } else {
            page_no_--;
        }
    }

    Game *game() const { return owns_->game(); }

private:
    TileViewScene *const owns_;
    int                  page_col_   = 0;
    int                  page_row_   = 0;
    int                  page_no_    = 0;
    float                font_scale_ = 0.7;
};  // class TileViewScene::Core

TileViewScene::TileViewScene(Game *game) : Scene(game), core_(new Core(this)) {}
TileViewScene::~TileViewScene() {}

void TileViewScene::Reset() {}
void TileViewScene::OnKeyInput(int key, int code, int action, int mods) {
    switch (key) {
        case GLFW_KEY_BACKSPACE:
            if (action == GLFW_PRESS) {
                DelayDispose();
                DCHECK_NOTNULL(prev())->SwitchTo(nullptr);
            }
            break;

        case GLFW_KEY_RIGHT:
        case GLFW_KEY_DOWN:
            if (action == GLFW_PRESS) { core_->NextPage(); }
            break;

        case GLFW_KEY_LEFT:
        case GLFW_KEY_UP:
            if (action == GLFW_PRESS) { core_->PrevPage(); }
            break;

        default: break;
    }
}

void TileViewScene::Render(double delta) {
    game()->transform()->Enter2DProjection();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    core_->DrawPage(delta);

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

    game()->transform()->Exit2DProjection();

    char buf[128];
    ::snprintf(buf, arraysize(buf), "%d/%d", core_->page_no() + 1, core_->MaxPageNo());

    res::FontFace *font = game()->font_lib()->default_face();
    Vector2f       size = font->ApproximateSize(buf);

    float x = game()->fb_w() - size.x;
    float y = game()->fb_h() - size.y;
    font->Render(buf, x, y);
}

}  // namespace nyaa
