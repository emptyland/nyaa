#pragma once
#ifndef NYAA_RESOURCE_CUBE_LIBRARY_H_
#define NYAA_RESOURCE_CUBE_LIBRARY_H_

#include "resource/cube-kinds-inl.h"
#include "game/identifiers.h"
#include "base/arena-utils.h"
#include <string>

namespace nyaa {

namespace res {

class Texture;
class TextureLibrary;

class Cube final : public base::ArenaObject {
public:
    enum Kind {
    #define ENUM_KIND(name) name,
        DEFINE_CUBE_KINDS(ENUM_KIND)
    #undef ENUM_KIND
        MAX_CUBE_KINDS,
    }; // enum Kind

    DEF_VAL_GETTER(Kind, kind);
    DEF_VAL_GETTER(ResourceId, id);
    DEF_PTR_GETTER(Texture, top_tex);
    DEF_PTR_GETTER(Texture, edge_tex);
    DEF_VAL_GETTER(std::string_view, name);

    friend class CubeLibrary;
private:
    Cube(Kind kind): kind_(kind) {}

    Kind kind_;
    ResourceId id_;
    Texture *top_tex_;
    Texture *edge_tex_;
    std::string_view name_;
}; // class Cube

class CubeLibrary final {
public:
    static const char kCubeDir[];
    static const char kCubeDefFileName[];

    CubeLibrary(TextureLibrary *tex_lib, base::Arena *arena);

    Cube *FindOrNull(ResourceId id) const {
        auto iter = id_to_cubes_.find(id);
        return iter == id_to_cubes_.end() ? nullptr : iter->second;
    }

    Cube *cube(Cube::Kind kind) const {
        int index = static_cast<int>(kind);
        DCHECK_GE(index, 0);
        DCHECK_LT(index, Cube::MAX_CUBE_KINDS);
        return cubes_[index];
    }

    bool Prepare(const std::string &file_name);

    DISALLOW_IMPLICIT_CONSTRUCTORS(CubeLibrary);
private:
    base::Arena *const arena_;
    TextureLibrary *const tex_lib_;
    base::ArenaUnorderedMap<ResourceId, Cube *> id_to_cubes_;
    Cube *cubes_[Cube::MAX_CUBE_KINDS];
}; // class CubeLibrary
    
} // namespace res
    
} // namespace nyaa


#endif // NYAA_RESOURCE_CUBE_LIBRARY_H_