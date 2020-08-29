#pragma once
#ifndef NYAA_RESOURCE_AVATAR_LIBRARY_H_
#define NYAA_RESOURCE_AVATAR_LIBRARY_H_

#include "game/vector.h"
#include "resource/resource-library.h"
#include <string>

namespace nyaa {

namespace res {

class Texture;
class TextureLibrary;
class AvatarLibrary;

class Avatar final : public base::ArenaObject {
public:
    enum Direction {
        kUp,
        kDown,
        kLeft,
        kRight,
        kMaxDir,
    };
    static constexpr int kMaxFrames = 8;

    Avatar(ResourceId id, Vector2f size, float speed, int frame_count);

    DEF_VAL_GETTER(ResourceId, id);
    DEF_VAL_GETTER(int, frames_count);
    DEF_VAL_GETTER(float, speed);
    DEF_VAL_GETTER(Vector2f, size);
    DEF_VAL_PROP_RW(int, vbo_hint);

    Texture *key_frame(Direction dir) const { return frame(dir, 0); }

    Texture *frame(Direction dir, int i) const {
        DCHECK_GE(i, 0);
        DCHECK_LT(i, frames_count_);
        return textures_[dir][i];
    }

    friend class AvatarLibrary;
    DISALLOW_IMPLICIT_CONSTRUCTORS(Avatar);

private:
    ResourceId id_;
    int        frames_count_;
    float      speed_;
    Vector2f   size_;
    int        vbo_hint_ = 0;
    int        key_frame_[kMaxDir];
    Texture *  textures_[kMaxDir][kMaxFrames];
};  // class Avatar

class AvatarLibrary final : public ResourceLibrary<Avatar, AvatarLibrary> {
public:
    static const char kAvatarDir[];
    static const char kAvatarDefFileName[];

    AvatarLibrary(TextureLibrary *tex_lib, base::Arena *arena);

    bool Load(DefinitionReader *rd);

    DISALLOW_IMPLICIT_CONSTRUCTORS(AvatarLibrary);
private:
    const TextureLibrary *const tex_lib_;
};  // class AvatarLibrary

}  // namespace res

}  // namespace nyaa

#endif  // NYAA_RESOURCE_AVATAR_LIBRARY_H_