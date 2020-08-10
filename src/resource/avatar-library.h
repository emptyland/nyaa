#pragma once
#ifndef NYAA_RESOURCE_AVATAR_LIBRARY_H_
#define NYAA_RESOURCE_AVATAR_LIBRARY_H_

#include "game/identifiers.h"
#include "base/arena-utils.h"
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

    Avatar(ResourceId id, Vertex2f size, float speed, int frame_count);

    DEF_VAL_GETTER(ResourceId, id);
    DEF_VAL_GETTER(int, frames_count);
    DEF_VAL_GETTER(float, speed);
    DEF_VAL_GETTER(Vertex2f, size);

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
    int frames_count_;
    float speed_;
    Vertex2f size_;
    int key_frame_[kMaxDir];
    Texture *textures_[kMaxDir][kMaxFrames];
}; // class Avatar
    
class AvatarLibrary final {
public:
    static const char kAvatarDir[];
    static const char kAvatarDefFileName[];

    AvatarLibrary(TextureLibrary *tex_lib, base::Arena *arena)
        : arena_(arena)
        , tex_lib_(tex_lib)
        , avatars_(arena) {}

    bool Prepare(const std::string &file_name);

    Avatar *FindOrNull(ResourceId id) const {
        auto iter = avatars_.find(id);
        return iter == avatars_.end() ? nullptr : iter->second;
    }
private:
    base::Arena *const arena_;
    TextureLibrary *const tex_lib_;
    base::ArenaUnorderedMap<ResourceId, Avatar*> avatars_;
}; // class AvatarLibrary

} // namespace res

} // namespace nyaa


#endif // NYAA_RESOURCE_AVATAR_LIBRARY_H_