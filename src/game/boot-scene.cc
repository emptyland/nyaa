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

static const float positions[6][4][3] = {
    // :format
    {{-1, -1, -1}, {-1, -1, +1}, {-1, +1, -1}, {-1, +1, +1}},
    // :format
    {{+1, -1, -1}, {+1, -1, +1}, {+1, +1, -1}, {+1, +1, +1}},
    // :format
    {{-1, +1, -1}, {-1, +1, +1}, {+1, +1, -1}, {+1, +1, +1}},
    // :format
    {{-1, -1, -1}, {-1, -1, +1}, {+1, -1, -1}, {+1, -1, +1}},
    // :format
    {{-1, -1, -1}, {-1, +1, -1}, {+1, -1, -1}, {+1, +1, -1}},
    // :format
    {{-1, -1, +1}, {-1, +1, +1}, {+1, -1, +1}, {+1, +1, +1}}};
static const float normals[6][3] = {
    // :format
    {-1, 0, 0},
    // :format
    {+1, 0, 0},
    // :format
    {0, +1, 0},
    // :format
    {0, -1, 0},
    // :format
    {0, 0, -1},
    // :format
    {0, 0, +1}};

static float vertices[] = {
    // top
    -1, -1, +1, /*normal*/ 0, 0, +1, /*uv*/ 0, 0, // :format
    -1, +1, +1, /*normal*/ 0, 0, +1, /*uv*/ 0, 0, // :format
    +1, +1, +1, /*normal*/ 0, 0, +1, /*uv*/ 0, 0, // :format
    +1, -1, +1, /*normal*/ 0, 0, +1, /*uv*/ 0, 0, // :format
    // bottom
    -1, -1, -1, /*normal*/ 0, 0, -1, /*uv*/ 0, 0, // :format
    -1, +1, -1, /*normal*/ 0, 0, -1, /*uv*/ 0, 0, // :format
    +1, +1, -1, /*normal*/ 0, 0, -1, /*uv*/ 0, 0, // :format
    +1, -1, -1, /*normal*/ 0, 0, -1, /*uv*/ 0, 0, // :format
    // :front
    -1, -1, -1, /*normal*/ 0, -1, 0, /*uv*/ 0, 0, // :format
    +1, -1, -1, /*normal*/ 0, -1, 0, /*uv*/ 0, 0, // :format
    +1, -1, +1, /*normal*/ 0, -1, 0, /*uv*/ 0, 0, // :format
    -1, -1, +1, /*normal*/ 0, -1, 0, /*uv*/ 0, 0, // :format
    // :back
    -1, +1, -1, /*normal*/ 0, +1, 0, /*uv*/ 0, 0, // :format
    -1, +1, +1, /*normal*/ 0, +1, 0, /*uv*/ 0, 0, // :format
    +1, +1, +1, /*normal*/ 0, +1, 0, /*uv*/ 0, 0, // :format
    +1, +1, -1, /*normal*/ 0, +1, 0, /*uv*/ 0, 0, // :format
    // :left
    -1, -1, -1, /*normal*/ -1, 0, 0, /*uv*/ 0, 0, // :format
    -1, -1, +1, /*normal*/ -1, 0, 0, /*uv*/ 0, 0, // :format
    -1, +1, +1, /*normal*/ -1, 0, 0, /*uv*/ 0, 0, // :format
    -1, +1, -1, /*normal*/ -1, 0, 0, /*uv*/ 0, 0, // :format
    // :right
    +1, -1, -1, /*normal*/ +1, 0, 0, /*uv*/ 0, 0, // :format
    +1, -1, +1, /*normal*/ +1, 0, 0, /*uv*/ 0, 0, // :format
    +1, +1, +1, /*normal*/ +1, 0, 0, /*uv*/ 0, 0, // :format
    +1, +1, -1, /*normal*/ +1, 0, 0, /*uv*/ 0, 0, // :format
};

BootScene::BootScene(Game *game) : Scene(game) {}

BootScene::~BootScene() {}

void BootScene::Reset() {
    res::Texture *tex = game()->texture_lib()->FindOrNull(ResourceId::Of(200120));

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

    GLuint pid      = game()->shader_lib()->demo_program();
    GLint  position = glGetAttribLocation(pid, "position");
    GLint  normal   = glGetAttribLocation(pid, "normal");
    GLint  uv       = glGetAttribLocation(pid, "uv");
    glUseProgram(pid);

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(position, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), nullptr);
    glVertexAttribPointer(normal, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    glVertexAttribPointer(uv, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          reinterpret_cast<void *>(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(position);
    glEnableVertexAttribArray(normal);
    glEnableVertexAttribArray(uv);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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
        y_rolated_ -= 0.5;
    } else if (glfwGetKey(game()->window(), GLFW_KEY_DOWN) == GLFW_PRESS) {
        y_rolated_ += 0.5;
    } else if (glfwGetKey(game()->window(), GLFW_KEY_LEFT) == GLFW_PRESS) {
        z_rolated_ -= 0.5;
    } else if (glfwGetKey(game()->window(), GLFW_KEY_RIGHT) == GLFW_PRESS) {
        z_rolated_ += 0.5;
    }

    GLuint pid          = game()->shader_lib()->demo_program();
    GLint  proj_matrix  = glGetUniformLocation(pid, "projectionMatrix");
    GLint  view_matrix  = glGetUniformLocation(pid, "viewMatrix");
    GLint  model_matrix = glGetUniformLocation(pid, "modelMatrix");
    GLint  sampler      = glGetUniformLocation(pid, "sampler");

    Matrix mat;
    mat.Identity();

    Matrix view_mat;
    view_mat.Translate(0, 0, -1);
    mat.Rotate(0, 1, 0, y_rolated_);
    view_mat.Multiply(mat);
    mat.Rotate(0, 0, 1, z_rolated_);
    view_mat.Multiply(mat);

    Matrix model_mat;
    model_mat.Scale(0.1, 0.1, 0.1);

    Matrix proj_mat;
    proj_mat.Perspective(45, static_cast<float>(game()->fb_w()) / game()->fb_h(), 0.1, 100);

    // proj_mat.Identity();
    // mat.Identity();
    // proj_mat.Multiply(mat);
    // proj_mat.Frustum(0, game()->fb_w(), 0, Game::This()->fb_h(), -100.0, 100.0);
    // proj_mat.Perspective(0, static_cast<float>(game()->fb_w())/game()->fb_h(), -100, 100);

    glUniformMatrix4fv(model_matrix, 1, GL_FALSE, model_mat.values());
    glUniformMatrix4fv(view_matrix, 1, GL_FALSE, view_mat.values());
    glUniformMatrix4fv(proj_matrix, 1, GL_FALSE, proj_mat.values());

    res::Texture *tex = game()->texture_lib()->FindOrNull(ResourceId::Of(200120));
    glBindTexture(GL_TEXTURE_2D, tex->tex_id());
    glBindVertexArray(vao_);
    glDrawArrays(GL_QUADS, 0, 24);
    glBindVertexArray(0);
}

}  // namespace nyaa
