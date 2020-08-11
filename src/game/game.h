#pragma once
#ifndef NYAA_GAME_GAME_H_
#define NYAA_GAME_GAME_H_

#include "game/identifiers.h"
#include "base/lazy-instance.h"
#include "base/arenas.h"
#include "base/slice.h"


using GLFWwindow = struct GLFWwindow;

namespace nyaa {
namespace res {
class FontLibrary;
class TextLibrary;
class TextureLibrary;
class AvatarLibrary;
class CubeLibrary;
} // namespace res

class Scene;
class Properties;

class Game final {
public:
    Game();
    ~Game();

    DEF_VAL_GETTER(int, window_h);
    DEF_VAL_GETTER(int, window_w);
    DEF_VAL_GETTER(int, fb_h);
    DEF_VAL_GETTER(int, fb_w);

    const Properties *properties() const { return properties_.get(); }
    res::FontLibrary *font_lib() const { return font_lib_.get(); }
    res::TextLibrary *text_lib() const { return text_lib_.get(); }
    res::AvatarLibrary *avatar_lib() const { return avatar_lib_.get(); }
    res::TextureLibrary *texture_lib() const { return texture_lib_.get(); }
    res::CubeLibrary *cube_lib() const { return cube_lib_.get(); }
    base::AbstractPrinter *debug_out() { return &stdout_; }

    bool Prepare(const std::string &properties_file_name);
    void Run();
    void Exit() { exit_ = true; }

    void SetWindowTitle(const char *title);

    EntityId NewEntityId() { return EntityId::Of(eid_generator_.New()); }
    EntityId NextEntityId() { return EntityId::Of(eid_generator_.Next()); }

    void EnterProjection2D();
    void LeaveProjection2D();
private:
    class IdGenerator {
    public:
        IdGenerator();
        uint64_t New();
        uint64_t Next() { return sequence_number_++; }
    private:
        uint16_t bucket_id_;
        uint64_t sequence_number_ = 0;
    };

    static void OnKeyInput(GLFWwindow *window,int key, int code, int action, int mods);
    static void OnMouseInput(GLFWwindow *window, double x, double y);

    base::StandaloneArena arena_;
    Scene *scene_ = nullptr;
    std::unique_ptr<Scene> boot_scene_;
    std::unique_ptr<res::FontLibrary> font_lib_;
    std::unique_ptr<res::TextLibrary> text_lib_;
    std::unique_ptr<res::TextureLibrary> texture_lib_;
    std::unique_ptr<res::AvatarLibrary> avatar_lib_;
    std::unique_ptr<res::CubeLibrary> cube_lib_;
    std::unique_ptr<Properties> properties_;
    base::StdFilePrinter stdout_;
    double ts_ = 0;
    GLFWwindow *window_ = nullptr;
    int window_h_ = 0;
    int window_w_ = 0;
    int fb_h_ = 0;
    int fb_w_ = 0;
    bool exit_ = false;
    IdGenerator eid_generator_;
}; // class Game

extern base::LazyInstance<Game> ThisGame;

class Projection2DScope final {
public:
    Projection2DScope(Game *game): owns_(game) { owns_->EnterProjection2D(); }
    ~Projection2DScope() { owns_->LeaveProjection2D(); }

    DISALLOW_IMPLICIT_CONSTRUCTORS(Projection2DScope);
private:
    Game *const owns_;
};
    
} // namespace nyaa


#endif // NYAA_GAME_GAME_H_