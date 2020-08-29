#pragma once
#ifndef NYAA_RESOURCE_SPRITE_LIBRARY_H_
#define NYAA_RESOURCE_SPRITE_LIBRARY_H_

#include "resource/resource-library.h"
#include "game/vector.h"

namespace nyaa {

namespace res {

class TextureLibrary;
class Texture;

class Sprite : public base::ArenaObject {
public:
    static constexpr int kMaxFrames = 8;

    DEF_VAL_GETTER(ResourceId, id);
    DEF_VAL_GETTER(int, frames_count);
    DEF_VAL_GETTER(float, speed);
    DEF_VAL_GETTER(Vector3f, light);
    DEF_VAL_GETTER(Vector3f, ambient_meterial);
    DEF_VAL_GETTER(Vector3f, diffuse_meterial);
    DEF_VAL_GETTER(Vector3f, specular_meterial);

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
    Sprite(ResourceId id, int frames_count, float speed, Vector3f light, Vector3f ambient_meterial,
           Vector3f diffuse_meterial, Vector3f specular_meterial)
        : id_(id)
        , frames_count_(frames_count)
        , speed_(speed)
        , light_(light)
        , ambient_meterial_(ambient_meterial)
        , diffuse_meterial_(diffuse_meterial)
        , specular_meterial_(specular_meterial) {
        int i = kMaxFrames;
        while (i--) { frames_[i] = nullptr; }
    }

    const ResourceId id_;
    const int        frames_count_;
    const float      speed_;
    const Vector3f   light_;
    // material(ambient/diffuse/specular)
    const Vector3f ambient_meterial_;
    const Vector3f diffuse_meterial_;
    const Vector3f specular_meterial_;

    int      vbo_hint_ = 0;
    Texture *frames_[kMaxFrames];
};  // class Sprite

class SpriteLibrary final : public ResourceLibrary<Sprite, SpriteLibrary> {
public:
    static const char kSpriteDir[];
    static const char kSpriteDefFileName[];

    SpriteLibrary(TextureLibrary *tex_lib, base::Arena *arena);

    bool Load(DefinitionReader *rd);

private:
    const TextureLibrary *const tex_lib_;
};  // class DecorationLibrary

}  // namespace res

}  // namespace nyaa

#endif  // NYAA_RESOURCE_SPRITE_LIBRARY_H_