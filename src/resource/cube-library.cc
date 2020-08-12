#include "resource/cube-library.h"
#include "resource/texture-library.h"
#include "resource/definition.h"

namespace nyaa {

namespace res {

class CubeDef final : public Definition<CubeDef> {
public:
    DEF_VAL_GETTER(ResourceId, id);
    DEF_VAL_GETTER(std::string, name);
    DEF_VAL_GETTER(ResourceId, top_tex_id);
    DEF_VAL_GETTER(ResourceId, edge_tex_id);

    void Parse(const std::vector<std::string_view> &items) {
        ParseValue<DefValType::ID>(items[0], &id_);
        ParseValue<DefValType::STRING>(items[1], &name_);
        ParseValue<DefValType::ID>(items[2], &top_tex_id_);
        ParseValue<DefValType::ID>(items[3], &edge_tex_id_);
    }

private:
    ResourceId  id_;
    std::string name_;
    ResourceId  top_tex_id_;
    ResourceId  edge_tex_id_;
    // TODO:
};  // class CubeDef

const char CubeLibrary::kCubeDir[]         = "textures";
const char CubeLibrary::kCubeDefFileName[] = "textures/cube.txt";

CubeLibrary::CubeLibrary(TextureLibrary *tex_lib, base::Arena *arena)
    : arena_(arena), tex_lib_(tex_lib), id_to_cubes_(arena) {
    ::memset(cubes_, 0, sizeof(cubes_));
}

bool CubeLibrary::Prepare(const std::string &file_name) {
    FILE *fp = ::fopen(file_name.c_str(), "r");
    if (!fp) {
        DLOG(ERROR) << "can not open cube definition file: " << file_name;
        return false;
    }

    int              kind = 0;
    DefinitionReader rd(fp, true /*ownership*/);
    CubeDef          row;
    while (row.Read(&rd) != EOF) {
        if (id_to_cubes_.find(row.id()) != id_to_cubes_.end()) {
            DLOG(ERROR) << "Duplicated cube id: " << row.id().value();
            return false;
        }
        if (kind >= Cube::MAX_CUBE_KINDS) {
            DLOG(ERROR) << "Cube resource file out of bound(" << kind << ")";
            return false;
        }

        Cube *cube  = new (arena_) Cube(static_cast<Cube::Kind>(kind++));
        cube->id_   = row.id();
        cube->name_ = "";
        if (cube->top_tex_ = tex_lib_->FindOrNull(row.top_tex_id()); !cube->top_tex_) {
            DLOG(ERROR) << "Texture id " << row.top_tex_id().value() << " not found for cube definition.";
            return false;
        }
        if (cube->edge_tex_ = tex_lib_->FindOrNull(row.edge_tex_id()); !cube->edge_tex_) {
            DLOG(ERROR) << "Texture id " << row.edge_tex_id().value() << " not found for cube definition.";
            return false;
        }

        cubes_[cube->kind()] = cube;
    }
    return true;
}

}  // namespace res

}  // namespace nyaa