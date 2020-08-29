#pragma once
#ifndef NYAA_COMPONENT_AVATAR_COMPONENT_H_
#define NYAA_COMPONENT_AVATAR_COMPONENT_H_

#include "resource/avatar-library.h"

namespace nyaa {

namespace com {

class AvatarComponent {
public:
    AvatarComponent(res::Avatar *def) : def_(def) {}

    DEF_PTR_GETTER(res::Avatar, def);
    DEF_VAL_PROP_RW(res::Avatar::Direction, dir);
    DEF_VAL_PROP_RW(double, speed);
    DEF_VAL_PROP_RW(double, time);

    res::Texture *GetFrame(double delta) {
        AddTime(delta);
        return GetFrame();
    }

    res::Texture *GetFrame() const {
        int i = static_cast<int>(time_);
        if (speed_ == 0.0) { return def_->key_frame(dir_); }
        return def_->frame(dir_, i % (def_->frames_count() - 1) + 1);
    }

    int FrameIndex(double delta) {
        int i = AddTime(delta);
        if (speed_ == 0.0) { return static_cast<int>(dir_) * def_->frames_count() + 0; }
        return static_cast<int>(dir_) * def_->frames_count() + i % (def_->frames_count() - 1) + 1;
    }

    DISALLOW_IMPLICIT_CONSTRUCTORS(AvatarComponent);

private:
    int AddTime(double delta) {
        time_ += delta * speed();
        if (time_ >= def_->frames_count() + 1) { time_ = 0; }
        return static_cast<int>(time_);
    }

    res::Avatar::Direction dir_ = res::Avatar::kDown;
    res::Avatar *          def_;
    double                 speed_ = 0;
    double                 time_  = 0;
};  // class AvatarComponent

}  // namespace com

}  // namespace nyaa

#endif  // NYAA_COMPONENT_AVATAR_COMPONENT_H_