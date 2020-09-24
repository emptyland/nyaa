#include "scene/avatar-view-scene.h"
#include "system/geometry-transform-system.h"
#include "resource/avatar-library.h"
#include "resource/texture-library.h"
#include "resource/font-library.h"
#include "game/game.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

class AvatarViewScene::Core {
public:
    static constexpr int kBorderSize  = 48;
    static constexpr int kPaddingSize = 48;
    static constexpr int kAvatarW     = 80;

    void Prepare(Game *game) {
        for (auto [id, def] : game->avatar_lib()->resource()) { avatars_.push_back(def); }
        std::sort(avatars_.begin(), avatars_.end(), [](res::Avatar *a, res::Avatar *b) { return a->id() < b->id(); });

        UpdatePageSize(game);
    }

    void UpdatePageSize(Game *game) {
        int w     = game->fb_w() - kBorderSize * 2;
        int h     = game->fb_h() - kBorderSize * 2;
        page_col_ = w / (avatar_w_ + kPaddingSize);
        page_row_ = h / (AvatarH() + kPaddingSize);

        page_no_ = std::min(page_no_, MaxPageNo() - 1);
    }

    int AvatarH() const { return avatars_.front()->key_frame(res::Avatar::kDown)->aspect_ratio() * avatar_w_; }

    int PageSize() const { return page_col_ * page_row_; }

    int MaxPageNo() const { return (avatars_.size() + PageSize() - 1) / PageSize(); }

    void NextPage() { page_no_ = (page_no_ + 1) % MaxPageNo(); }

    void PrevPage() {
        if (page_no_ == 0) {
            page_no_ = MaxPageNo() - 1;
        } else {
            page_no_ = (page_no_ - 1) % MaxPageNo();
        }
    }

    void DrawPage(Game *game, double delta) {
        UpdatePageSize(game);

        time_ += delta * 3;

        res::FontFace *font = game->font_lib()->system_face();

        int offset = page_no_ * PageSize();
        for (int j = 0; j < page_row_; j++) {
            for (int i = 0; i < page_col_; i++) {
                int index = offset++;
                if (index >= avatars_.size()) { return; }

                float x = i * (avatar_w_ + kPaddingSize) + kBorderSize;
                float y = game->fb_h() - (j + 1) * (AvatarH() + kPaddingSize) - kBorderSize;

                DrawAvatar(x, y, avatars_[index], font);
            }
        }
    }

    void DrawAvatar(float x, float y, res::Avatar *def, res::FontFace *font) {
        char buf[64];
        ::snprintf(buf, arraysize(buf), "%d", def->id().value());
        Vector2f size = font->ApproximateSize(buf);
        size.x *= font_scale_;
        size.y *= font_scale_;

        Vector3f pos;
        pos.x = x + avatar_w_ / 2;
        pos.x -= size.x / 2;
        pos.y = y - 32;
        pos.z = 0;

        std::vector<float> vertices;
        font->Render(pos, font_scale_, buf, &vertices);

        res::Texture *tex = nullptr;
        if (run_) {
            tex = def->frame(dir_, static_cast<int>(time_) % (def->frames_count() - 1) + 1);
            // tex = def->frame(dir_, static_cast<int>(time_) % def->frames_count());
        } else {
            tex = def->key_frame(dir_);
        }
        glBindTexture(GL_TEXTURE_2D, tex->tex_id());

        glBegin(GL_QUADS);
        glColor3f(1.0, 1.0, 1.0);
        glTexCoord2fv(&tex->coord(3).x);
        glVertex2f(x, y);

        glTexCoord2fv(&tex->coord(0).x);
        glVertex2f(x, y + AvatarH());

        glTexCoord2fv(&tex->coord(1).x);
        glVertex2f(x + avatar_w_, y + AvatarH());

        glTexCoord2fv(&tex->coord(2).x);
        glVertex2f(x + avatar_w_, y);

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

    std::vector<res::Avatar *> avatars_;

    int   page_col_   = 0;
    int   page_row_   = 0;
    int   page_no_    = 0;
    int   avatar_w_   = kAvatarW;
    float font_scale_ = 0.7;
    float time_       = 0.0;
    bool  run_        = false;

    res::Avatar::Direction dir_ = res::Avatar::kDown;

private:
};  // class AvatarViewScene::Controller

AvatarViewScene::AvatarViewScene(Game *game) : Scene(game), ctrl_(new Core) {}

AvatarViewScene::~AvatarViewScene() {}

void AvatarViewScene::Reset() { ctrl_->Prepare(game()); }

void AvatarViewScene::OnKeyInput(int key, int code, int action, int mods) {
    switch (key) {
        case GLFW_KEY_BACKSPACE:
            if (action == GLFW_PRESS) {
                DelayDispose();
                DCHECK_NOTNULL(prev())->SwitchTo(nullptr);
            }
            break;

        case GLFW_KEY_W:
            if (action == GLFW_PRESS) { ctrl_->dir_ = res::Avatar::kUp; }
            break;

        case GLFW_KEY_S:
            if (action == GLFW_PRESS) { ctrl_->dir_ = res::Avatar::kDown; }
            break;

        case GLFW_KEY_A:
            if (action == GLFW_PRESS) { ctrl_->dir_ = res::Avatar::kLeft; }
            break;

        case GLFW_KEY_D:
            if (action == GLFW_PRESS) { ctrl_->dir_ = res::Avatar::kRight; }
            break;

        case GLFW_KEY_F:
            if (action == GLFW_PRESS) { ctrl_->run_ = !ctrl_->run_; }
            break;

        case GLFW_KEY_RIGHT:
        case GLFW_KEY_DOWN:
            if (action == GLFW_PRESS) { ctrl_->NextPage(); }
            break;

        case GLFW_KEY_LEFT:
        case GLFW_KEY_UP:
            if (action == GLFW_PRESS) { ctrl_->PrevPage(); }
            break;

        default: break;
    }
}

void AvatarViewScene::Render(double delta) {
    game()->transform()->Enter2DProjection();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ctrl_->DrawPage(game(), delta);

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

    game()->transform()->Exit2DProjection();

    char buf[128];
    ::snprintf(buf, arraysize(buf), "%d/%d", ctrl_->page_no_ + 1, ctrl_->MaxPageNo());

    res::FontFace *font = game()->font_lib()->default_face();
    Vector2f       size = font->ApproximateSize(buf);

    float x = game()->fb_w() - size.x;
    float y = game()->fb_h() - size.y;
    font->Render(buf, x, y);
}

}  // namespace nyaa
