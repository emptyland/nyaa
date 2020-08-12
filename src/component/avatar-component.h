#pragma once
#ifndef NYAA_COMPONENT_AVATAR_COMPONENT_H_
#define NYAA_COMPONENT_AVATAR_COMPONENT_H_

#include "resource/avatar-library.h"

namespace nyaa {

namespace com {

class AvatarComponent {
public:
    AvatarComponent(res::Avatar *def) : def_(def) {}

    DEF_VAL_PROP_RW(res::Avatar::Direction, dir);
    DEF_VAL_PROP_RW(double, speed);
    DEF_VAL_PROP_RW(double, time);

    res::Texture *GetFrame() const {
        if (speed_ == 0.0) { return def_->key_frame(dir_); }
        int i = static_cast<int>(time_ / (speed_ * def_->speed())) % (def_->frames_count() - 1) + 1;
        return def_->frame(dir_, i);
    }

    void AddTime(double d) { time_ += d; }

    DISALLOW_IMPLICIT_CONSTRUCTORS(AvatarComponent);

private:
    res::Avatar::Direction dir_ = res::Avatar::kDown;
    res::Avatar *          def_;
    double                 speed_ = 0;
    double                 time_  = 0;
};  // class AvatarComponent

}  // namespace com

}  // namespace nyaa

#endif  // NYAA_COMPONENT_AVATAR_COMPONENT_H_