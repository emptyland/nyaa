#include "game/game.h"
#include "game/properties.h"
#include "game/boot-scene.h"
#include "game/scene.h"
#include "resource/definition.h"
#include "resource/font-library.h"
#include "resource/text-library.h"
#include "resource/texture-library.h"
#include "resource/avatar-library.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <sys/time.h>

namespace nyaa {

base::LazyInstance<Game> ThisGame;

Game::Game()
    : boot_scene_(new BootScene(this))
    , font_lib_(new res::FontLibrary(&arena_))
    , text_lib_(new res::TextLibrary(&arena_))
    , texture_lib_(new res::TextureLibrary(&arena_))
    , avatar_lib_(new res::AvatarLibrary(texture_lib_.get(), &arena_))
    , properties_(new Properties())
    , stdout_(stdout) {
    // Total initialize
    ::glfwInit();
}

Game::~Game() {
    ::glfwTerminate();
}

void Game::SetWindowTitle(const char *title) {
    ::glfwSetWindowTitle(window_, title);
}

bool Game::Prepare(const std::string &properties_file_name) {
    FILE *fp = ::fopen(properties_file_name.c_str(), "rb");
    if (fp == nullptr) {
        DLOG(ERROR) << "can not open properties file: " << properties_file_name;
        return false;
    }
    res::DefinitionReader rd(fp, true/*ownership*/);
    while (properties_->Read(&rd) != EOF) {}

    if (!text_lib_->Prepare(properties()->assets_dir() + "/" + properties()->language())) {
        return false;
    }
#ifdef DEBUG
    properties_->Print(debug_out());
#endif

    DCHECK(window_ == nullptr);
    window_ = glfwCreateWindow(properties()->window_width(),
        properties()->window_height(),
        properties()->name().c_str(), nullptr, nullptr);
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

    res::FontLibrary::Options options;
    options.default_font_file = properties()->assets_dir() + "/" + properties()->default_font_file();
    options.default_font_size = properties()->default_font_size();
    if (!font_lib_->LoadFaces(options)) {
        return false;
    }
    if (!texture_lib_->Prepare(properties()->assets_dir() + "/" + res::TextureLibrary::kTextureDefFileName)) {
        return false;
    }
    if (!avatar_lib_->Prepare(properties()->assets_dir() + "/" + res::AvatarLibrary::kAvatarDefFileName)) {
        return false;
    }

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

void Game::EnterProjection2D() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, ThisGame->fb_w(), 0, ThisGame->fb_h(), -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_CULL_FACE);
}

void Game::LeaveProjection2D() {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

} // namespace nyaa
