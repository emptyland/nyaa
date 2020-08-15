#include "game/game.h"
#include "game/properties.h"
#include "game/boot-scene.h"
#include "game/scene.h"
#include "system/entity-allocation-system.h"
#include "system/zone-render-system.h"
#include "system/random-zone-system.h"
#include "resource/definition.h"
#include "resource/font-library.h"
#include "resource/text-library.h"
#include "resource/texture-library.h"
#include "resource/avatar-library.h"
#include "resource/cube-library.h"
#include "glog/logging.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <sys/time.h>

namespace nyaa {

base::LazyInstance<Game> ThisGame;

Game::Game()
    : boot_scene_(new BootScene(this))
    , entity_allocator_(new sys::EntityAllocationSystem())
    , zone_render_(new sys::ZoneRenderSystem())
    , random_zone_(new sys::RandomZoneSystem())
    , font_lib_(new res::FontLibrary(&arena_))
    , text_lib_(new res::TextLibrary(&arena_))
    , texture_lib_(new res::TextureLibrary(&arena_))
    , avatar_lib_(new res::AvatarLibrary(texture_lib_.get(), &arena_))
    , cube_lib_(new res::CubeLibrary(texture_lib_.get(), &arena_))
    , properties_(new Properties())
    , stdout_(stdout) {
    // Total initialize
    ::glfwInit();
}

Game::~Game() { ::glfwTerminate(); }

void Game::SetWindowTitle(const char *title) { ::glfwSetWindowTitle(window_, title); }

void FrustumResizeCallback(GLFWwindow *window, int w, int h) {
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(0, w, 0, h, -100.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

bool Game::Prepare(const std::string &properties_file_name) {
    FILE *fp = ::fopen(properties_file_name.c_str(), "rb");
    if (fp == nullptr) {
        DLOG(ERROR) << "can not open properties file: " << properties_file_name;
        return false;
    }
    res::DefinitionReader rd(fp, true /*ownership*/);
    while (properties_->Read(&rd) != EOF) {}

    if (!text_lib_->Prepare(properties()->assets_dir() + "/" + properties()->language())) { return false; }
#ifdef DEBUG
    properties_->Print(debug_out());
#endif

    DCHECK(window_ == nullptr);
    window_ = glfwCreateWindow(properties()->window_width(), properties()->window_height(),
                               properties()->name().c_str(), nullptr, nullptr);
    if (!window_) {
        DLOG(ERROR) << "can not create window.";
        return false;
    }
    glfwMakeContextCurrent(window_);
    glfwSetWindowUserPointer(window_, this);
    glfwSetKeyCallback(window_, OnKeyInput);
    glfwSetCursorPosCallback(window_, OnMouseInput);
    // glfwSetWindowRefreshCallback(window_, FrustumResizeCallback);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);

    glfwSwapInterval(60);

    ts_ = glfwGetTime();

    res::FontLibrary::Options options;
    options.default_font_file = properties()->assets_dir() + "/" + properties()->default_font_file();
    options.default_font_size = properties()->default_font_size();
    if (!font_lib_->LoadFaces(options)) { return false; }
    if (!texture_lib_->Prepare(properties()->assets_dir() + "/" + res::TextureLibrary::kTextureDefFileName)) {
        return false;
    }
    if (!avatar_lib_->Prepare(properties()->assets_dir() + "/" + res::AvatarLibrary::kAvatarDefFileName)) {
        return false;
    }
    if (!cube_lib_->Prepare(properties()->assets_dir() + "/" + res::CubeLibrary::kCubeDefFileName)) { return false; }

    // Initial tiles texture id
    res::Texture *tex = DCHECK_NOTNULL(texture_lib_->FindOrNull(ResourceId::Of(200000)));
    zone_render()->set_tile_tex_id(tex->tex_id());

    scene_ = boot_scene_.get();
    boot_scene_->Reset();
    return true;
}

void Game::Run() {
    while (!glfwWindowShouldClose(window_) && !exit_) {
        double delta = glfwGetTime() - ts_;
        ts_          = glfwGetTime();

        glfwGetWindowSize(window_, &window_w_, &window_h_);
        glfwGetFramebufferSize(window_, &fb_w_, &fb_h_);

        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        scene_->Render(delta);

        if (properties()->show_fps()) {
            char fps_buf[120];
            ::snprintf(fps_buf, sizeof(fps_buf), "FPS: %0.2f", 1 / delta);
            Projection2DScope scope(this);
            font_lib()->default_face()->Render(fps_buf, 0, fb_h() - properties()->default_font_size(), {1, 1, 1});
        }

        glfwSwapBuffers(window_);
        glfwPollEvents();

        // Others process
        while (!recycle_scenes_.empty()) {
            Scene *junk = recycle_scenes_.front();
            recycle_scenes_.pop_front();

            DLOG(INFO) << "Delete junk scene: " << junk->Name();
            delete junk;
        }
    }

    glfwDestroyWindow(window_);
    window_ = nullptr;
}

/*static*/ void Game::OnKeyInput(GLFWwindow *window, int key, int code, int action, int mods) {
    Game *game = static_cast<Game *>(glfwGetWindowUserPointer(window));
    if (game->scene_) { game->scene_->OnKeyInput(key, code, action, mods); }
}

/*static*/ void Game::OnMouseInput(GLFWwindow *window, double x, double y) {
    Game *game = static_cast<Game *>(glfwGetWindowUserPointer(window));
    if (game->scene_) { game->scene_->OnMouseInput(x, y); }
}

void Game::DelayDeleteScene(Scene *scene) {
#ifndef NDEBUG
    auto iter = std::find(recycle_scenes_.begin(), recycle_scenes_.end(), scene);
    DCHECK(iter == recycle_scenes_.end());
#endif
    recycle_scenes_.push_back(DCHECK_NOTNULL(scene));
}

Game::IdGenerator::IdGenerator() : bucket_id_((::rand() & 0xffff0000) >> 16) {}

uint64_t Game::IdGenerator::New() {
    uint16_t seq = sequence_number_++;
    timeval  time_val;
    ::gettimeofday(&time_val, nullptr);
    uint64_t mills = time_val.tv_sec * 1000 + time_val.tv_usec / 1000;
    return (static_cast<uint64_t>(bucket_id_ & 0x3ff) << 54) | ((mills & 0x7ffffffffff) << 12) |
           (sequence_number_ & 0x1fff);
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

}  // namespace nyaa
