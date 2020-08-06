#include "game/game.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
//#include <Glut/glut.h>

namespace nyaa {

base::LazyInstance<Game> ThisGame;

Game::Game() {
    // Total initialize
    glfwInit();
}

Game::~Game() {

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

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);

    glfwSwapInterval(60);

    return true;
}

void Game::Run() {
    while (!glfwWindowShouldClose(window_) && !exit_) {
        auto last_ts = glfwGetTime();

        glfwGetWindowSize(window_, &window_w_, &window_h_);
        glfwGetFramebufferSize(window_, &fb_w_, &fb_h_);
        //FrustumResizeCallback(window, w, h);

        // TO 3D
        glViewport(0, 0, fb_w_, fb_h_);

        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        glfwSwapBuffers(window_);
        glfwPollEvents();
    }

    glfwDestroyWindow(window_);
    window_ = nullptr;
}

/*static*/ void Game::OnKeyInput(GLFWwindow *window, int key, int code, int action, int mods) {
    Game *game = static_cast<Game *>(glfwGetWindowUserPointer(window));
    switch (key) {
    case GLFW_KEY_ESCAPE:
        game->exit_ = true;
        break;
    
    default:
        break;
    }
}   

} // namespace nyaa
