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
#include "resource/shader-library.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace nyaa {

BootScene::BootScene(Game *game) : Scene(game) {}

BootScene::~BootScene() {}

void BootScene::Reset() {
    glGenBuffers(1, &vbo_[0]);
    glGenBuffers(1, &vbo_[1]);
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

void BootScene::Render(double d) {
    // const Vertex2i fb_size{Game::This()->fb_w(), Game::This()->fb_h()};
    // glViewport(0, 0, fb_size.x, fb_size.y);

    // game()->transform()->EnterRotatedProjection();

    // GLuint pid = game()->shader_lib()->simple_light_program();
    // glUseProgram(pid);

    // //glEnableClientState(GL_VERTEX_ARRAY);
    // GLint position = glGetAttribLocation(pid, "position");
    // GLint normal = glGetAttribLocation(pid, "normal");
    // GLint projection_matrix = glGetUniformLocation(pid, "projectionMatrix");
    // GLint modelview_matrix = glGetUniformLocation(pid, "modelViewMatrix");

    // float pmat[16] = {0};
    // glGetFloatv(GL_PROJECTION, pmat);
    // float mvmat[16] = {0};
    // glGetFloatv(GL_MODELVIEW, mvmat);

    // //glUniformMatrix4fv(projection_matrix, 1, GL_FALSE, pmat);
    // glUniformMatrix4fv(modelview_matrix, 1, GL_FALSE, mvmat);

    // glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    // glEnableVertexAttribArray(position);
    // glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
    // glEnableVertexAttribArray(normal);
    // glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // //glUseProgram(game()->shader_lib()->simple_light_program());

    // glColor3f(1,1,1);
    // glDrawArrays(GL_TRIANGLES, 0, 36);

    // glUseProgram(0);
    // game()->transform()->ExitRotatedProjection();
}

}  // namespace nyaa
