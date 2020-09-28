#include "scene/cube-view-scene.h"
#include "system/geometry-transform-system.h"
#include "resource/texture-library.h"
#include "resource/font-library.h"
#include "resource/shader-library.h"
#include "resource/cube-library.h"
#include "game/game.h"
#include "game/vector.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

class CubeViewScene::Core {
public:
    static constexpr int   kCubeSize    = res::Cube::MAX_CUBE_KINDS;
    static constexpr auto  kTileId      = ResourceId::Of(200000);
    static constexpr float kPaddingSize = 0.6;
    static constexpr float kScaleFactor = 200.0;
    static constexpr float kTextScale   = 0.6;

    struct TextLabel {
        Vector2f    screen_coord;
        Vector2f    size;
        const char *text;
        int         vbo_hint;
        int         vbo_count;
    };

    Core(CubeViewScene *owns) : owns_(owns) {}
    ~Core() { glDeleteBuffers(2, vbo_); }

    void Prepare() {
        if (initialized_) { return; }
        glGenBuffers(2, vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);

        std::vector<float> buf;
        buf.reserve(kCubeSize * 24 * 8);

        for (int i = 0; i < kCubeSize; i++) {
            size_t pos = buf.size();
            buf.resize(pos + 24 * 8);
            float *vertices = &buf[pos];

            FillCubeBuf(game()->cube_lib()->cube(static_cast<res::Cube::Kind>(i)), vertices);
        }
        glBufferData(GL_ARRAY_BUFFER, buf.size() * sizeof(float), &buf[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
        buf.clear();
        res::FontFace *font = game()->font_lib()->default_face();
        for (int i = 0; i < kCubeSize; i++) {
            cube_labels_[i].text = res::Cube::IndexToName(i);
            cube_labels_[i].size = font->ApproximateSize(cube_labels_[i].text);

            cube_labels_[i].vbo_hint = static_cast<int>(buf.size());
            font->Render(Vec3(0, 0, 0), 1.0, cube_labels_[i].text, &buf);
            cube_labels_[i].vbo_count = buf.size() - cube_labels_[i].vbo_hint;

            cube_labels_[i].vbo_hint /= 5;
            cube_labels_[i].vbo_count /= 5;
        }
        glBufferData(GL_ARRAY_BUFFER, buf.size() * sizeof(float), &buf[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        block_shader_ = game()->shader_lib()->block_program();
        tile_tex_id_  = game()->texture_lib()->FindOrNull(kTileId)->tex_id();

        text_shader_ = game()->shader_lib()->text_program();
        text_tex_id_ = font->buffered_tex();

        offset_.x = scale_;
        offset_.y = scale_;

        const int w  = ::sqrt(kCubeSize) + 1;
        total_row_   = w;
        total_col_   = w;
        initialized_ = true;
    }

    void DrawPage(double delta) {
        block_shader_->Use();
        SetUpShader();

        glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
        block_shader_->Enable();

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tile_tex_id_);

        int index = 0;
        for (int j = 0; j < total_row_; j++) {
            for (int i = 0; i < total_col_; i++) {
                if (index >= kCubeSize) { goto cube_done; }

                DrawCube(index++, i, j);
            }
        }
        // DrawCube(1, 0, 0);
    cube_done:
        block_shader_->Disable();

        glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);

        text_shader_->Use();
        text_shader_->Enable();

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glEnable(GL_LINE_SMOOTH);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, text_tex_id_);

        Matrix<float> projection_mat;
        projection_mat.Ortho(0, game()->fb_w(), 0, game()->fb_h(), -1, 10);
        text_shader_->SetProjectionMatrix(projection_mat);
        text_shader_->SetTextColor(Vec4(1, 1, 1, 1));

        index = 0;
        for (int j = 0; j < total_row_; j++) {
            for (int i = 0; i < total_col_; i++) {
                if (index >= kCubeSize) { goto text_done; }

                DrawTags(index);
                index++;
            }
        }

    text_done:
        text_shader_->Disable();
        text_shader_->Unuse();

        glDisable(GL_BLEND);
        glDisable(GL_LINE_SMOOTH);
        glEnable(GL_DEPTH_TEST);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void HandleInput() {
        const float rolated_speed = 2;

        if (owns_->TestKeyPressed(GLFW_KEY_UP)) {
            AddZRolated(rolated_speed);
        } else if (owns_->TestKeyPressed(GLFW_KEY_DOWN)) {
            AddZRolated(-rolated_speed);
        } else if (owns_->TestKeyPressed(GLFW_KEY_LEFT)) {
            AddYRolated(-rolated_speed);
        } else if (owns_->TestKeyPressed(GLFW_KEY_RIGHT)) {
            AddYRolated(rolated_speed);
        } else if (owns_->TestKeyPressed(GLFW_KEY_K)) {
            scale_ += 0.001;
        } else if (owns_->TestKeyPressed(GLFW_KEY_L)) {
            scale_ -= 0.001;
            if (scale_ < 0.1) { scale_ = 0.1; }
        } else if (owns_->TestKeyPressed(GLFW_KEY_W)) {
            offset_.y += 0.01;
        } else if (owns_->TestKeyPressed(GLFW_KEY_S)) {
            offset_.y -= 0.01;
        } else if (owns_->TestKeyPressed(GLFW_KEY_A)) {
            offset_.x += 0.01;
        } else if (owns_->TestKeyPressed(GLFW_KEY_D)) {
            offset_.x -= 0.01;
        }
    }

private:
    void DrawCube(int index, int col, int row) {
        if (index >= kCubeSize) { return; }

        float x = -total_col_ * (scale_ + kPaddingSize) / 2 + col * (scale_ + kPaddingSize) + offset_.x;
        float y = -total_row_ * (scale_ + kPaddingSize) / 2 + row * (scale_ + kPaddingSize) + offset_.y;

        model_mat_.Identity();
        Matrix<float> mat;

        mat.Translate(x, y, 0);
        model_mat_.Multiply(mat);

        mat.Rotate(0, 1, 0, y_rolated_);
        model_mat_.Multiply(mat);

        mat.Rotate(0, 0, 1, z_rolated_);
        model_mat_.Multiply(mat);

        mat.Scale(scale_, scale_, scale_);
        model_mat_.Multiply(mat);

        block_shader_->SetModelMatrix(model_mat_);

        Vector4f coord = System::This()->transform()->TransformToScreen(
            projection_mat_, view_mat_, model_mat_, Vec4(0, 0, 0, 1), {game()->fb_w(), game()->fb_h()});

        cube_labels_[index].screen_coord = Vec2(coord.x, coord.y);

        glDrawArrays(GL_QUADS, index * 24, 24);
    }

    void DrawTags(int index) {
        Matrix<float> view_model_mat;
        view_model_mat.Identity();

        const TextLabel *label = &cube_labels_[index];
        Vector2f         size  = label->size * kTextScale;
        float            x     = label->screen_coord.x - size.x / 2;
        float            y     = label->screen_coord.y - size.y - 72;

        Matrix<float> mat;
        mat.Translate(x, y, 0);
        view_model_mat.Multiply(mat);
        mat.Scale(kTextScale, kTextScale, kTextScale);
        view_model_mat.Multiply(mat);

        text_shader_->SetViewModelMatrix(view_model_mat);

        glDrawArrays(GL_QUADS, label->vbo_hint, label->vbo_count);
        // DLOG(INFO) << label->text << ", hint = " << label->vbo_hint << ", count = " << label->vbo_count
        //            << ", x = " << label->screen_coord.x << ", y = " << label->screen_coord.y;
    }

    void SetUpShader() {
        projection_mat_.Perspective(45, static_cast<float>(game()->fb_w()) / game()->fb_h(), 0.1, 100);
        view_mat_.Translate(camera_.x, camera_.y, camera_.z);
        // view_mat_.Identity();

        block_shader_->SetProjectionMatrix(projection_mat_);
        block_shader_->SetViewMatrix(view_mat_);
        block_shader_->SetDiffuseMaterial({0.6, 0.6, 0.6});
        block_shader_->SetAmbientMaterial({0.8, 0.8, 0.8});
        block_shader_->SetSpecularMaterial({0.7, 0.7, 0.7});
        block_shader_->SetDirectionalLight(directional_light_);
        block_shader_->SetDirectionalLightColor(Vec3(ambient_light_, ambient_light_, ambient_light_));
        block_shader_->SetPointLightColor(Vec3(0, 0, 0));
        block_shader_->SetPointLightConstant(1);
        block_shader_->SetPointLightLinear(0.09);
        block_shader_->SetPointLightQuadratic(0.032);
        block_shader_->SetPointLightPosition(Vec3(0, 0, 2));
        block_shader_->SetCameraPosition({-camera_.x, -camera_.y, -camera_.z});
    }

    static void FillCubeBuf(const res::Cube *cube, float *vertices) {
        for (int i = 0; i < 24; i++) {
            vertices[i * 8 + 0] = kVertices[i * 8 + 0];
            vertices[i * 8 + 1] = kVertices[i * 8 + 1];
            vertices[i * 8 + 2] = kVertices[i * 8 + 2];

            vertices[i * 8 + 3] = kVertices[i * 8 + 3];
            vertices[i * 8 + 4] = kVertices[i * 8 + 4];
            vertices[i * 8 + 5] = kVertices[i * 8 + 5];
        }
        for (int i = 0; i < 4; i++) {
            vertices[i * 8 + 6] = cube->top_tex()->coord(i).x;
            vertices[i * 8 + 7] = cube->top_tex()->coord(i).y;
        }
        for (int i = 4; i < 24; i++) {
            vertices[i * 8 + 6] = cube->edge_tex()->coord(i % 4).x;
            vertices[i * 8 + 7] = cube->edge_tex()->coord(i % 4).y;
        }
    }

    void AddYRolated(float angle) { y_rolated_ += angle; }
    void AddZRolated(float angle) { z_rolated_ += angle; }

    Game *game() const { return owns_->game(); }

    CubeViewScene *const     owns_;
    bool                     initialized_ = false;
    GLuint                   vbo_[2];
    GLuint                   tile_tex_id_  = 0;
    GLuint                   text_tex_id_  = 0;
    res::BlockShaderProgram *block_shader_ = nullptr;
    res::TextShaderProgram * text_shader_  = nullptr;

    Matrix<float> projection_mat_;
    Matrix<float> model_mat_;
    Matrix<float> view_mat_;

    float    ambient_light_     = 0.6;
    float    y_rolated_         = 45;
    float    z_rolated_         = 45;
    Vector4f camera_            = {0, 0, -2, 1};
    Vector3f directional_light_ = {0, 1, -1};

    int   total_row_ = 0;
    int   total_col_ = 0;
    float scale_     = 0.2;

    Vector2f offset_ = {0, 0};

    TextLabel cube_labels_[kCubeSize];

    static const float kVertices[];
};  // class CubeViewScene::Core

const float CubeViewScene::Core::kVertices[] = {
    // top
    -0.5, -0.5, 0.5, /*normal*/ 0, 0, 1, /*uv*/ 0, 0,  // :format
    -0.5, 0.5, 0.5, /*normal*/ 0, 0, 1, /*uv*/ 0, 0,   // :format
    0.5, 0.5, 0.5, /*normal*/ 0, 0, 1, /*uv*/ 0, 0,    // :format
    0.5, -0.5, 0.5, /*normal*/ 0, 0, 1, /*uv*/ 0, 0,   // :format
    // bottom
    -0.5, -0.5, -0.5, /*normal*/ 0, 0, -1, /*uv*/ 0, 0,  // :format
    -0.5, 0.5, -0.5, /*normal*/ 0, 0, -1, /*uv*/ 0, 0,   // :format
    0.5, 0.5, -0.5, /*normal*/ 0, 0, -1, /*uv*/ 0, 0,    // :format
    0.5, -0.5, -0.5, /*normal*/ 0, 0, -1, /*uv*/ 0, 0,   // :format
    // :front
    -0.5, -0.5, -0.5, /*normal*/ 0, -1, 0, /*uv*/ 0, 0,  // :format
    -0.5, -0.5, 0.5, /*normal*/ 0, -1, 0, /*uv*/ 0, 0,   // :format
    0.5, -0.5, 0.5, /*normal*/ 0, -1, 0, /*uv*/ 0, 0,    // :format
    0.5, -0.5, -0.5, /*normal*/ 0, -1, 0, /*uv*/ 0, 0,   // :format
    // :back
    -0.5, 0.5, -0.5, /*normal*/ 0, 1, 0, /*uv*/ 0, 0,  // :format
    0.5, 0.5, -0.5, /*normal*/ 0, 1, 0, /*uv*/ 0, 0,   // :format
    0.5, 0.5, 0.5, /*normal*/ 0, 1, 0, /*uv*/ 0, 0,    // :format
    -0.5, 0.5, 0.5, /*normal*/ 0, 1, 0, /*uv*/ 0, 0,   // :format
    // :left
    -0.5, -0.5, -0.5, /*normal*/ -1, 0, 0, /*uv*/ 0, 0,  // :format
    -0.5, 0.5, -0.5, /*normal*/ -1, 0, 0, /*uv*/ 0, 0,   // :format
    -0.5, 0.5, 0.5, /*normal*/ -1, 0, 0, /*uv*/ 0, 0,    // :format
    -0.5, -0.5, 0.5, /*normal*/ -1, 0, 0, /*uv*/ 0, 0,   // :format
    // :right
    0.5, -0.5, -0.5, /*normal*/ 1, 0, 0, /*uv*/ 0, 0,  // :format
    0.5, -0.5, 0.5, /*normal*/ 1, 0, 0, /*uv*/ 0, 0,   // :format
    0.5, 0.5, 0.5, /*normal*/ 1, 0, 0, /*uv*/ 0, 0,    // :format
    0.5, 0.5, -0.5, /*normal*/ 1, 0, 0, /*uv*/ 0, 0,   // :format
};

CubeViewScene::CubeViewScene(Game *game) : Scene(game), core_(new Core(this)) {}
CubeViewScene::~CubeViewScene() {}

void CubeViewScene::Reset() { core_->Prepare(); }
void CubeViewScene::OnKeyInput(int key, int code, int action, int mods) {
    switch (key) {
        case GLFW_KEY_BACKSPACE:
            if (action == GLFW_PRESS) {
                DelayDispose();
                DCHECK_NOTNULL(prev())->SwitchTo(nullptr);
            }
            break;

        default: break;
    }
}

void CubeViewScene::Render(double delta) {
    core_->HandleInput();
    core_->DrawPage(delta);
}

}  // namespace nyaa
