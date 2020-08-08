#pragma once
#ifndef NYAA_GAME_GAME_H_
#define NYAA_GAME_GAME_H_

#include "game/identifiers.h"
#include "base/lazy-instance.h"
#include "base/slice.h"
#include "base/base.h"

using GLFWwindow = struct GLFWwindow;

namespace nyaa {
namespace res {
class FontLibrary;
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
    base::AbstractPrinter *debug_out() { return &stdout_; }

    bool Prepare(const std::string &properties_file_name);
    void Run();
    void Exit() { exit_ = true; }

    void SetWindowTitle(const char *title);

    EntityId NewEntityId() { return EntityId::Of(eid_generator_.New()); }
    EntityId NextEntityId() { return EntityId::Of(eid_generator_.Next()); }
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

    Scene *scene_ = nullptr;
    std::unique_ptr<Scene> boot_scene_;
    std::unique_ptr<res::FontLibrary> font_lib_;
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
    
} // namespace nyaa


#endif // NYAA_GAME_GAME_H_