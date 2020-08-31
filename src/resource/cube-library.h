#pragma once
#ifndef NYAA_RESOURCE_CUBE_LIBRARY_H_
#define NYAA_RESOURCE_CUBE_LIBRARY_H_

#include "resource/cube-def-inl.h"
#include "resource/resource-library.h"
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
    };  // enum Kind

    DEF_VAL_GETTER(Kind, kind);
    DEF_VAL_GETTER(ResourceId, id);
    DEF_PTR_GETTER(Texture, top_tex);
    DEF_PTR_GETTER(Texture, edge_tex);
    DEF_VAL_GETTER(int, vw);
    DEF_VAL_GETTER(int, vh);
    DEF_VAL_GETTER(std::string_view, name);

    friend class CubeLibrary;

private:
    Cube(Kind kind) : kind_(kind) {}

    Kind             kind_;
    ResourceId       id_;
    Texture *        top_tex_;
    Texture *        edge_tex_;
    int              vw_;
    int              vh_;
    std::string_view name_;
};  // class Cube

class CubeLibrary final : public ResourceLibrary<Cube, CubeLibrary> {
public:
    static const char kCubeDir[];
    static const char kCubeDefFileName[];

    CubeLibrary(TextureLibrary *tex_lib, base::Arena *arena);

    Cube *cube(Cube::Kind kind) const {
        int index = static_cast<int>(kind);
        DCHECK_GE(index, 0);
        DCHECK_LT(index, Cube::MAX_CUBE_KINDS);
        return cubes_[index];
    }

    bool Load(DefinitionReader *rd);

    DISALLOW_IMPLICIT_CONSTRUCTORS(CubeLibrary);

private:
    const TextureLibrary *const tex_lib_;
    Cube *                      cubes_[Cube::MAX_CUBE_KINDS];
};  // class CubeLibrary

}  // namespace res

}  // namespace nyaa

#endif  // NYAA_RESOURCE_CUBE_LIBRARY_H_