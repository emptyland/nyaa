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
    static constexpr int  kCubeSize    = res::Cube::MAX_CUBE_KINDS;
    static constexpr auto kTileId      = ResourceId::Of(200000);
    static constexpr int  kBorderSize  = 48;
    static constexpr int  kPaddingSize = 48;
    static constexpr int  kCubeW       = 128;
    static constexpr int  kCubeH       = 128;

    Core(CubeViewScene *owns) : owns_(owns) {}
    ~Core() { glDeleteBuffers(1, &vbo_); }

    void Prepare() {
        if (initialized_) { return; }
        glGenBuffers(1, &vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);

        std::vector<float> buf;
        buf.reserve(kCubeSize * 24 * 8);

        for (int i = 0; i < kCubeSize; i++) {
            size_t pos = buf.size();
            buf.resize(pos + 24 * 8);
            float *vertices = &buf[pos];

            FillCubeBuf(game()->cube_lib()->cube(static_cast<res::Cube::Kind>(i)), vertices);
        }
        glBufferData(GL_ARRAY_BUFFER, buf.size() * sizeof(float), &buf[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        shader_ = game()->shader_lib()->block_program();

        tile_tex_id_ = game()->texture_lib()->FindOrNull(kTileId)->tex_id();

        projection_mat_.Perspective(45, static_cast<float>(game()->fb_w()) / game()->fb_h(), 0.1, 100);
        view_mat_.Translate(0, 0, -2);
        initialized_ = true;
    }

    void DrawPage(double delta) {
        shader_->Use();
        SetUpShader();

        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        shader_->Enable();

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tile_tex_id_);

        DrawCube(1, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        shader_->Disable();
        shader_->Unuse();
    }

    Game *game() const { return owns_->game(); }

private:
    void DrawCube(int index, int col, int row) {
        if (index >= kCubeSize) { return; }

        model_mat_.Identity();
        Matrix<float> mat;
        mat.Rotate(1, 0, 0, y_rolated_);
        model_mat_.Multiply(mat);
        mat.Rotate(0, 0, 1, z_rolated_);
        model_mat_.Multiply(mat);
        mat.Scale(0.2, 0.2, 0.2);
        model_mat_.Multiply(mat);

        shader_->SetModelMatrix(model_mat_);

        glDrawArrays(GL_QUADS, index * 24, 24);
    }

    void UpdatePageSize() {}

    void SetUpShader() {
        shader_->SetProjectionMatrix(projection_mat_);
        shader_->SetViewMatrix(view_mat_);
        shader_->SetDiffuseMaterial({0.6, 0.6, 0.6});
        shader_->SetAmbientMaterial({0.8, 0.8, 0.8});
        shader_->SetSpecularMaterial({0.7, 0.7, 0.7});
        shader_->SetDirectionalLight(directional_light_);
        shader_->SetDirectionalLightColor(Vec3(ambient_light_, ambient_light_, ambient_light_));
        shader_->SetPointLightColor(Vec3(0, 0, 0));
        shader_->SetPointLightConstant(1);
        shader_->SetPointLightLinear(0.09);
        shader_->SetPointLightQuadratic(0.032);
        shader_->SetPointLightPosition(Vec3(0, 0, 2));
        shader_->SetCameraPosition({camera_.x, camera_.y, camera_.z});
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

    CubeViewScene *const     owns_;
    bool                     initialized_ = false;
    GLuint                   vbo_         = 0;
    GLuint                   tile_tex_id_ = 0;
    res::BlockShaderProgram *shader_      = nullptr;
    int                      page_col_    = 0;
    int                      page_row_    = 0;
    int                      page_no_     = 0;

    Matrix<float> projection_mat_;
    Matrix<float> model_mat_;
    Matrix<float> view_mat_;

    float    ambient_light_     = 0.8;
    float    y_rolated_         = 45;
    float    z_rolated_         = 45;
    Vector4f camera_            = {0, 0, -2, 1};
    Vector3f directional_light_ = {0, 1, -1};

    float scale_ = 0.2;

    static const float kVertices[];
};  // class CubeViewScene::Core

const float CubeViewScene::Core::kVertices[] = {
    // top
    0, 0, 1, /*normal*/ 0, 0, 1, /*uv*/ 0, 0,  // :format
    0, 1, 1, /*normal*/ 0, 0, 1, /*uv*/ 0, 0,  // :format
    1, 1, 1, /*normal*/ 0, 0, 1, /*uv*/ 0, 0,  // :format
    1, 0, 1, /*normal*/ 0, 0, 1, /*uv*/ 0, 0,  // :format
    // bottom
    0, 0, 0, /*normal*/ 0, 0, -1, /*uv*/ 0, 0,  // :format
    0, 1, 0, /*normal*/ 0, 0, -1, /*uv*/ 0, 0,  // :format
    1, 1, 0, /*normal*/ 0, 0, -1, /*uv*/ 0, 0,  // :format
    1, 0, 0, /*normal*/ 0, 0, -1, /*uv*/ 0, 0,  // :format
    // :front
    0, 0, 0, /*normal*/ 0, -1, 0, /*uv*/ 0, 0,  // :format
    0, 0, 1, /*normal*/ 0, -1, 0, /*uv*/ 0, 0,  // :format
    1, 0, 1, /*normal*/ 0, -1, 0, /*uv*/ 0, 0,  // :format
    1, 0, 0, /*normal*/ 0, -1, 0, /*uv*/ 0, 0,  // :format
    // :back
    0, 1, 0, /*normal*/ 0, 1, 0, /*uv*/ 0, 0,  // :format
    1, 1, 0, /*normal*/ 0, 1, 0, /*uv*/ 0, 0,  // :format
    1, 1, 1, /*normal*/ 0, 1, 0, /*uv*/ 0, 0,  // :format
    0, 1, 1, /*normal*/ 0, 1, 0, /*uv*/ 0, 0,  // :format
    // :left
    0, 0, 0, /*normal*/ -1, 0, 0, /*uv*/ 0, 0,  // :format
    0, 1, 0, /*normal*/ -1, 0, 0, /*uv*/ 0, 0,  // :format
    0, 1, 1, /*normal*/ -1, 0, 0, /*uv*/ 0, 0,  // :format
    0, 0, 1, /*normal*/ -1, 0, 0, /*uv*/ 0, 0,  // :format
    // :right
    1, 0, 0, /*normal*/ 1, 0, 0, /*uv*/ 0, 0,  // :format
    1, 0, 1, /*normal*/ 1, 0, 0, /*uv*/ 0, 0,  // :format
    1, 1, 1, /*normal*/ 1, 0, 0, /*uv*/ 0, 0,  // :format
    1, 1, 0, /*normal*/ 1, 0, 0, /*uv*/ 0, 0,  // :format
};

CubeViewScene::CubeViewScene(Game *game) : Scene(game), core_(new Core(this)) {}
CubeViewScene::~CubeViewScene() {}

void CubeViewScene::Reset() { core_->Prepare(); }
void CubeViewScene::OnKeyInput(int key, int code, int action, int mods) {}

void CubeViewScene::Render(double delta) {
    // res::BlockShaderProgram *shader = game()->shader_lib()->block_program();
    // shader->Use();
    // shader->Enable();

    core_->DrawPage(delta);

    // shader->Disable();
    // shader->Unuse();
}

}  // namespace nyaa
