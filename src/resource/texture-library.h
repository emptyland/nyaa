#pragma once
#ifndef NYAA_RESOURCE_TEXTURE_LIBRARY_H_
#define NYAA_RESOURCE_TEXTURE_LIBRARY_H_

#include "game/identifiers.h"
#include "base/arena-utils.h"
#include "base/base.h"
#include "glog/logging.h"

namespace nyaa {

namespace res {

class Texture : public base::ArenaObject {
public:
    Texture(ResourceId id, uint32_t tex_id, int index, float aspect_ratio, Vector2f p0, Vector2f p1, Vector2f p2,
            Vector2f p3)
        : id_(id), tex_id_(tex_id), index_(index), aspect_ratio_(aspect_ratio) {
        coord_[0] = p0;
        coord_[1] = p1;
        coord_[2] = p2;
        coord_[3] = p3;
    }

    DEF_VAL_GETTER(ResourceId, id);
    DEF_VAL_GETTER(uint32_t, tex_id);
    DEF_VAL_GETTER(int, index);
    DEF_VAL_GETTER(float, aspect_ratio);

    const Vector2f &coord(int i) const {
        DCHECK_GE(i, 0);
        DCHECK_LT(i, 4);
        return coord_[i];
    }

private:
    ResourceId id_;
    uint32_t   tex_id_;
    int        index_;
    float      aspect_ratio_; // h/w ratio
    Vector2f   coord_[4];
};  // class Texture

class TextureLibrary final {
public:
    static const char kTextureDir[];
    static const char kTextureDefFileName[];

    TextureLibrary(base::Arena *arena) : arena_(arena), textures_(arena), texture_ids_(arena) {}

    ~TextureLibrary();

    bool Prepare(const std::string &file_name);

    Texture *FindOrNull(ResourceId id) const {
        auto iter = textures_.find(id);
        return iter == textures_.end() ? nullptr : iter->second;
    }

    DISALLOW_IMPLICIT_CONSTRUCTORS(TextureLibrary);

private:
    uint32_t LoadPictureFile(const std::string &name, Vector2f *size);

    base::Arena *const                             arena_;
    base::ArenaUnorderedMap<ResourceId, Texture *> textures_;
    base::ArenaVector<uint32_t>                    texture_ids_;
};  // class TextureLibrary

}  // namespace res

}  // namespace nyaa

#endif  // NYAA_RESOURCE_TEXTURE_LIBRARY_H_
