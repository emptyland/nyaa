#pragma once
#ifndef NYAA_GAME_GAME_H_
#define NYAA_GAME_GAME_H_

#include "base/lazy-instance.h"
#include "base/base.h"

using GLFWwindow = struct GLFWwindow;

namespace nyaa {

class Game final {
public:
    Game();
    ~Game();

    DEF_VAL_GETTER(int, window_h);
    DEF_VAL_GETTER(int, window_w);
    DEF_VAL_GETTER(int, fb_h);
    DEF_VAL_GETTER(int, fb_w);

    bool Prepare();
    void Run();
private:
    static void OnKeyInput(GLFWwindow *window,int key, int code, int action, int mods);

    GLFWwindow *window_ = nullptr;
    int window_h_ = 0;
    int window_w_ = 0;
    int fb_h_ = 0;
    int fb_w_ = 0;
    bool exit_ = false;
}; // class Game

extern base::LazyInstance<Game> ThisGame;
    
} // namespace nyaa


#endif // NYAA_GAME_GAME_H_