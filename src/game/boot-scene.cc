#include "game/boot-scene.h"
#include "game/test-scene.h"
#include "game/identifiers.h"
#include "game/matrix.h"
#include "game/game.h"
#include "component/avatar-component.h"
#include "component/cube-component.h"
#include "system/geometry-transform-system.h"
#include "resource/texture-library.h"
#include "resource/font-library.h"
#include "resource/avatar-library.h"
#include "resource/shader-library.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

static float vertices[] = {
    // top
    -1, -1, +1, /*normal*/ 0, 0, +1, /*uv*/ 0, 0,  // :format
    -1, +1, +1, /*normal*/ 0, 0, +1, /*uv*/ 0, 0,  // :format
    +1, +1, +1, /*normal*/ 0, 0, +1, /*uv*/ 0, 0,  // :format
    +1, -1, +1, /*normal*/ 0, 0, +1, /*uv*/ 0, 0,  // :format
    // bottom
    -1, -1, -1, /*normal*/ 0, 0, -1, /*uv*/ 0, 0,  // :format
    -1, +1, -1, /*normal*/ 0, 0, -1, /*uv*/ 0, 0,  // :format
    +1, +1, -1, /*normal*/ 0, 0, -1, /*uv*/ 0, 0,  // :format
    +1, -1, -1, /*normal*/ 0, 0, -1, /*uv*/ 0, 0,  // :format
    // :front
    -1, -1, -1, /*normal*/ 0, -1, 0, /*uv*/ 0, 0,  // :format
    +1, -1, -1, /*normal*/ 0, -1, 0, /*uv*/ 0, 0,  // :format
    +1, -1, +1, /*normal*/ 0, -1, 0, /*uv*/ 0, 0,  // :format
    -1, -1, +1, /*normal*/ 0, -1, 0, /*uv*/ 0, 0,  // :format
    // :back
    -1, +1, -1, /*normal*/ 0, +1, 0, /*uv*/ 0, 0,  // :format
    -1, +1, +1, /*normal*/ 0, +1, 0, /*uv*/ 0, 0,  // :format
    +1, +1, +1, /*normal*/ 0, +1, 0, /*uv*/ 0, 0,  // :format
    +1, +1, -1, /*normal*/ 0, +1, 0, /*uv*/ 0, 0,  // :format
    // :left
    -1, -1, -1, /*normal*/ -1, 0, 0, /*uv*/ 0, 0,  // :format
    -1, -1, +1, /*normal*/ -1, 0, 0, /*uv*/ 0, 0,  // :format
    -1, +1, +1, /*normal*/ -1, 0, 0, /*uv*/ 0, 0,  // :format
    -1, +1, -1, /*normal*/ -1, 0, 0, /*uv*/ 0, 0,  // :format
    // :right
    +1, -1, -1, /*normal*/ +1, 0, 0, /*uv*/ 0, 0,  // :format
    +1, -1, +1, /*normal*/ +1, 0, 0, /*uv*/ 0, 0,  // :format
    +1, +1, +1, /*normal*/ +1, 0, 0, /*uv*/ 0, 0,  // :format
    +1, +1, -1, /*normal*/ +1, 0, 0, /*uv*/ 0, 0,  // :format
};

BootScene::BootScene(Game *game) : Scene(game) {}

BootScene::~BootScene() {}

void BootScene::Reset() {
    res::Texture *tex = game()->texture_lib()->FindOrNull(ResourceId::Of(200140));

    for (int i = 0; i < 6; i++) {
        vertices[i * 32 + 6]  = tex->coord(0).x;
        vertices[i * 32 + 7]  = tex->coord(0).y;
        vertices[i * 32 + 14] = tex->coord(1).x;
        vertices[i * 32 + 15] = tex->coord(1).y;
        vertices[i * 32 + 22] = tex->coord(2).x;
        vertices[i * 32 + 23] = tex->coord(2).y;
        vertices[i * 32 + 30] = tex->coord(3).x;
        vertices[i * 32 + 31] = tex->coord(3).y;
    }

    // res::DemoShaderProgram *program = game()->shader_lib()->demo_program();
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    float billboard[] = {
        -1, -1, 0, /*uv*/ tex->coord(0).x, tex->coord(0).y,  // :format
        +1, -1, 0, /*uv*/ tex->coord(1).x, tex->coord(1).y,  // :format
        +1, +1, 0, /*uv*/ tex->coord(2).x, tex->coord(2).y,  // :format
        -1, +1, 0, /*uv*/ tex->coord(3).x, tex->coord(3).y,  // :format
    };

    glGenBuffers(1, &billboard_vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(billboard), billboard, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BootScene::OnKeyInput(int key, int code, int action, int mods) {
    switch (key) {
        case GLFW_KEY_ESCAPE: game()->Exit(); break;

        case GLFW_KEY_T: {
            TestScene *test_scene = new TestScene(game());
            test_scene->SwitchTo(this);
        } break;

        case GLFW_KEY_UP:
            // :format
            game()->transform()->set_rotate_angle_y(game()->transform()->rotate_angle_y() + 2);
            break;
        case GLFW_KEY_DOWN: game()->transform()->set_rotate_angle_y(game()->transform()->rotate_angle_y() - 2); break;
        case GLFW_KEY_LEFT: game()->transform()->set_rotate_angle_z(game()->transform()->rotate_angle_z() - 2); break;
        case GLFW_KEY_RIGHT: game()->transform()->set_rotate_angle_z(game()->transform()->rotate_angle_z() + 2); break;

        default: break;
    }
}

void BootScene::OnMouseInput(double x, double y) {}

void BootScene::Render(double d) {
    glViewport(0, 0, game()->fb_w(), game()->fb_w());
    glClearColor(0.2, 0.2, 0.4, 0.0);

    if (glfwGetKey(game()->window(), GLFW_KEY_UP) == GLFW_PRESS) {
        z_rolated_ -= 2;
    } else if (glfwGetKey(game()->window(), GLFW_KEY_DOWN) == GLFW_PRESS) {
        z_rolated_ += 2;
    } else if (glfwGetKey(game()->window(), GLFW_KEY_LEFT) == GLFW_PRESS) {
        y_rolated_ -= 2;
    } else if (glfwGetKey(game()->window(), GLFW_KEY_RIGHT) == GLFW_PRESS) {
        y_rolated_ += 2;
    }

    Matrix mat;
    mat.Identity();

    Matrix view_mat;
    view_mat.Translate(0, 0, -2);
    mat.Rotate(0, 1, 0, y_rolated_);
    view_mat.Multiply(mat);
    mat.Rotate(0, 0, 1, z_rolated_);
    view_mat.Multiply(mat);
    mat.Scale(0.1, 0.1, 0.1);
    view_mat.Multiply(mat);

    Matrix model_mat;
    model_mat.Identity();
    // model_mat.Rotate(0, 1, 0, y_rolated_);
    // mat.Rotate(0, 0, 1, z_rolated_);
    // model_mat.Multiply(mat);
    // mat.Scale(0.1, 0.1, 0.1);
    // model_mat.Multiply(mat);

    Matrix proj_mat;
    proj_mat.Perspective(45, static_cast<float>(game()->fb_w()) / game()->fb_h(), 0.1, 100);

    res::DemoShaderProgram *demo = game()->shader_lib()->demo_program();
    demo->Use();
    demo->SetProjectionMatrix(proj_mat);
    demo->SetViewMatrix(view_mat);
    demo->SetModelMatrix(model_mat);

    res::Texture *tex = game()->texture_lib()->FindOrNull(ResourceId::Of(200120));
    glBindTexture(GL_TEXTURE_2D, tex->tex_id());
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    demo->Enable();
    glDrawArrays(GL_QUADS, 0, 24);
    demo->Disable();
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    res::BillboardShaderProgram *billboard = game()->shader_lib()->billboard_program();
    billboard->Use();
    billboard->SetProjectionMatrix(proj_mat);
    billboard->SetViewMatrix(view_mat);
    //model_mat.Identity();
    //model_mat.Translate(0, 0, -1);
    //billboard->SetModelMatrix(model_mat);
    billboard->SetCenterPosition(Vec3(0, 0, 0));
    billboard->SetSize(Vec2(0.2, 0.2));

    glBindTexture(GL_TEXTURE_2D, tex->tex_id());
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vbo_);
    billboard->Enable();
    glDrawArrays(GL_QUADS, 0, 4);
    billboard->Disable();
    billboard->Unuse();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

}  // namespace nyaa
