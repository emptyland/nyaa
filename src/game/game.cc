#include "game/boot-scene.h"
#include "game/scene.h"
#include "game/game.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <sys/time.h>

namespace nyaa {

base::LazyInstance<Game> ThisGame;

Game::Game()
    : boot_scene_(new BootScene(this)) {
    // Total initialize
    glfwInit();
}

Game::~Game() {

}

void Game::SetWindowTitle(const char *title) {
    glfwSetWindowTitle(window_, title);
}

bool Game::Prepare() {

    DCHECK(window_ == nullptr);
    window_ = glfwCreateWindow(300, 300, "demo,demo,demo", nullptr, nullptr);
    if (!window_) {
        DLOG(ERROR) << "can not create window.";
        return false;
    }
    glfwMakeContextCurrent(window_);
    glfwSetWindowUserPointer(window_, this);
    glfwSetKeyCallback(window_, OnKeyInput);
    glfwSetCursorPosCallback(window_, OnMouseInput);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);

    glfwSwapInterval(60);

    ts_ = glfwGetTime();

    scene_ = boot_scene_.get();
    boot_scene_->Reset();
    return true;
}

void Game::Run() {
    while (!glfwWindowShouldClose(window_) && !exit_) {
        double delta = glfwGetTime() - ts_;
        ts_ = glfwGetTime();

        glfwGetWindowSize(window_, &window_w_, &window_h_);
        glfwGetFramebufferSize(window_, &fb_w_, &fb_h_);

        // TO 3D
        glViewport(0, 0, fb_w_, fb_h_);

        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        scene_->Render(delta);

        glfwSwapBuffers(window_);
        glfwPollEvents();
    }

    glfwDestroyWindow(window_);
    window_ = nullptr;
}

/*static*/ void Game::OnKeyInput(GLFWwindow *window, int key, int code, int action, int mods) {
    Game *game = static_cast<Game *>(glfwGetWindowUserPointer(window));
    if (game->scene_) {
        game->scene_->OnKeyInput(key, code, action, mods);
    }
}

/*static*/ void Game::OnMouseInput(GLFWwindow *window, double x, double y) {
    Game *game = static_cast<Game *>(glfwGetWindowUserPointer(window));
    if (game->scene_) {
        game->scene_->OnMouseInput(x, y);
    }
}

Game::IdGenerator::IdGenerator()
    : bucket_id_((::rand() & 0xffff0000) >> 16) {
}

uint64_t Game::IdGenerator::New() {
    uint16_t seq = sequence_number_++;
    timeval time_val;
    ::gettimeofday(&time_val, nullptr);
    uint64_t mills = time_val.tv_sec * 1000 + time_val.tv_usec / 1000;
    return (static_cast<uint64_t>(bucket_id_ & 0x3ff) << 54) 
        | ((mills & 0x7ffffffffff) << 12)
        | (sequence_number_ & 0x1fff);
}

} // namespace nyaa
