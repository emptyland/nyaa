#pragma once
#ifndef NYAA_RESOURCE_SPRITE_LIBRARY_H_
#define NYAA_RESOURCE_SPRITE_LIBRARY_H_

#include "game/identifiers.h"
#include "base/arena-utils.h"
#include "base/base.h"

namespace nyaa {

namespace res {

class TextureLibrary;
class Texture;

class Sprite : public base::ArenaObject {
public:
    static constexpr int kMaxFrames = 8;

    Sprite(ResourceId id, int frames_count, float speed, Vector3f light)
        : id_(id), frames_count_(frames_count), speed_(speed), light_(light) {
        int i = kMaxFrames;
        while (i--) { frames_[i] = nullptr; }
    }

    DEF_VAL_GETTER(ResourceId, id);
    DEF_VAL_GETTER(int, frames_count);
    DEF_VAL_GETTER(float, speed);
    DEF_VAL_GETTER(Vector3f, light);

    DEF_VAL_PROP_RW(int, vbo_hint);

    Texture *texture() const { return frame(0); }

    Texture *frame(int i) const {
        DCHECK_GE(i, 0);
        DCHECK_LT(i, frames_count());
        return DCHECK_NOTNULL(frames_[i]);
    }

    friend class SpriteLibrary;
    DISALLOW_IMPLICIT_CONSTRUCTORS(Sprite);

private:
    const ResourceId id_;
    const int        frames_count_;
    const float      speed_;
    const Vector3f   light_;
    int              vbo_hint_ = 0;
    Texture *        frames_[kMaxFrames];
};  // class Sprite


class SpriteLibrary final {
public:
    static const char kSpriteDir[];
    static const char kSpriteDefFileName[];

    SpriteLibrary(TextureLibrary *tex_lib, base::Arena *arena) : arena_(arena), tex_lib_(tex_lib), sprites_(arena) {}

    const base::ArenaUnorderedMap<ResourceId, Sprite *> &sprites() const { return sprites_; }

    bool Prepare(const std::string &file_name);

    Sprite *FindOrNull(ResourceId id) const {
        auto iter = sprites_.find(id);
        return iter == sprites_.end() ? nullptr : iter->second;
    }
private:
    base::Arena *const    arena_;
    TextureLibrary *const tex_lib_;

    base::ArenaUnorderedMap<ResourceId, Sprite *> sprites_;
};  // class DecorationLibrary

}  // namespace res

}  // namespace nyaa

#endif  // NYAA_RESOURCE_SPRITE_LIBRARY_H_