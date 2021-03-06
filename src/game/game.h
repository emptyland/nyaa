#pragma once
#ifndef NYAA_GAME_GAME_H_
#define NYAA_GAME_GAME_H_

#include "game/pseudo-random-number.h"
#include "game/identifiers.h"
#include "base/lazy-instance.h"
#include "base/arenas.h"
#include "base/slice.h"
#include <deque>

using GLFWwindow = struct GLFWwindow;

namespace nyaa {
namespace res {
class FontLibrary;
class TextLibrary;
class TextureLibrary;
class AvatarLibrary;
class CubeLibrary;
class ShaderLibrary;
class SpriteLibrary;
class ActorLibrary;
class SkillLibrary;
}  // namespace res

namespace ui {
class UIService;
}  // namespace ui

namespace sys {
class System;
}  // namespace sys

class Scene;
class Properties;
class Game;
class World;

class Game final {
public:
    Game();
    ~Game();

    DEF_VAL_GETTER(int, window_h);
    DEF_VAL_GETTER(int, window_w);
    DEF_VAL_GETTER(int, fb_h);
    DEF_VAL_GETTER(int, fb_w);
    DEF_VAL_GETTER(double, frame_delta_time);
    DEF_PTR_PROP_RW(Scene, scene);
    DEF_PTR_GETTER(GLFWwindow, window);
    DEF_VAL_GETTER(float, dpi_factor);
    DEF_VAL_GETTER(double, ts);
    DEF_VAL_PROP_RW(bool, show_fps);

    bool break_input() const;

    const Properties *properties() const { return properties_.get(); }

    PseudoRandomGenerator *random() { return &random_; }

    sys::System *system() const { return system_.get(); }

    res::FontLibrary *   font_lib() const { return font_lib_.get(); }
    res::TextLibrary *   text_lib() const { return text_lib_.get(); }
    res::AvatarLibrary * avatar_lib() const { return avatar_lib_.get(); }
    res::TextureLibrary *texture_lib() const { return texture_lib_.get(); }
    res::SpriteLibrary * sprite_lib() const { return sprite_lib_.get(); }
    res::SkillLibrary *  skill_lib() const { return skill_lib_.get(); }
    res::CubeLibrary *   cube_lib() const { return cube_lib_.get(); }
    res::ShaderLibrary * shader_lib() const { return shader_lib_.get(); }
    res::ActorLibrary *  actor_lib() const { return actor_lib_.get(); }

    World *world() const { return world_.get(); }

    base::AbstractPrinter *debug_out() { return &stdout_; }

    bool Prepare(const std::string &properties_file_name);
    void Run();
    void Exit() { exit_ = true; }

    void SetWindowTitle(const char *title);

    EntityId NewEntityId() { return EntityId::Of(eid_generator_.New()); }
    EntityId NextEntityId() { return EntityId::Of(eid_generator_.Next()); }

    static inline Game *This();

    void DelayDeleteScene(Scene *scene);

    void AddUIService(ui::UIService *service) { ui_services_.push_back(service); }

    void RemoveUIService(ui::UIService *service) {
        auto iter = std::find(ui_services_.begin(), ui_services_.end(), service);
        if (iter != ui_services_.end()) { ui_services_.erase(iter); }
    }

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

    class UIController;
    class CommandDispatcher;

    static void OnKeyInput(GLFWwindow *window, int key, int code, int action, int mods);
    static void OnCharInput(GLFWwindow *window, unsigned int codepoint);
    static void OnMouseButtonInput(GLFWwindow *window, int button, int action, int mods);
    static void OnMouseMove(GLFWwindow *window, double x, double y);

    void ProcessConsoleCommand(std::string_view text);

    base::StandaloneArena  arena_;
    Scene *                scene_ = nullptr;
    std::unique_ptr<Scene> boot_scene_;

    // System
    std::unique_ptr<sys::System> system_;

    // Resource libraries
    std::unique_ptr<res::FontLibrary>    font_lib_;
    std::unique_ptr<res::TextLibrary>    text_lib_;
    std::unique_ptr<res::TextureLibrary> texture_lib_;
    std::unique_ptr<res::SpriteLibrary>  sprite_lib_;
    std::unique_ptr<res::AvatarLibrary>  avatar_lib_;
    std::unique_ptr<res::CubeLibrary>    cube_lib_;
    std::unique_ptr<res::ShaderLibrary>  shader_lib_;
    std::unique_ptr<res::SkillLibrary>   skill_lib_;
    std::unique_ptr<res::ActorLibrary>   actor_lib_;

    std::unique_ptr<UIController> console_ui_;

    std::vector<ui::UIService *> ui_services_;

    std::unique_ptr<World> world_;

    std::unique_ptr<Properties> properties_;
    std::deque<Scene *>         recycle_scenes_;
    base::StdFilePrinter        stdout_;

    PseudoRandomGenerator random_;

    bool        break_input_      = false;
    float       dpi_factor_       = 1.0;
    double      ts_               = 0;
    double      frame_delta_time_ = 0;
    GLFWwindow *window_           = nullptr;
    int         window_h_         = 0;
    int         window_w_         = 0;
    int         fb_h_             = 0;
    int         fb_w_             = 0;
    bool        show_fps_         = false;
    bool        exit_             = false;
    IdGenerator eid_generator_;
};  // namespace nyaa

inline base::LazyInstance<Game> standalone_game;

/*static*/ inline Game *Game::This() { return standalone_game.Get(); }

}  // namespace nyaa

#endif  // NYAA_GAME_GAME_H_