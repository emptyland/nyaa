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
    DEF_VAL_GETTER(int, vw);
    DEF_VAL_GETTER(int, vh);

    void Parse(const std::vector<std::string_view> &items) {
        ParseValue<DefValType::ID>(items[0], &id_);
        ParseValue<DefValType::STRING>(items[1], &name_);
        ParseValue<DefValType::ID>(items[2], &top_tex_id_);
        ParseValue<DefValType::ID>(items[3], &edge_tex_id_);
        // items[4]: env
        ParseValue<DefValType::I32>(items[5], &vw_);
        ParseValue<DefValType::I32>(items[6], &vh_);
    }

private:
    ResourceId  id_;
    std::string name_;
    ResourceId  top_tex_id_;
    ResourceId  edge_tex_id_;
    // std::string env_;
    int vw_;
    int vh_;
    // TODO:
};  // class CubeDef

const char CubeLibrary::kCubeDir[]         = "";
const char CubeLibrary::kCubeDefFileName[] = "cube.txt";

CubeLibrary::CubeLibrary(TextureLibrary *tex_lib, base::Arena *arena) : ResourceLibrary(arena), tex_lib_(tex_lib) {
    ::memset(cubes_, 0, sizeof(cubes_));
}

bool CubeLibrary::Load(DefinitionReader *rd) {
    int     kind = 0;
    CubeDef row;
    while (row.Read(rd) != EOF) {
        if (FindOrNull(row.id())) {
            DLOG(ERROR) << "Duplicated cube id: " << row.id().value();
            return false;
        }
        if (kind >= Cube::MAX_CUBE_KINDS) {
            DLOG(ERROR) << "Cube resource file out of bound(" << kind << ")";
            return false;
        }

        Cube *cube  = new (arena()) Cube(static_cast<Cube::Kind>(kind++));
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
        cube->vw_ = row.vw();
        cube->vh_ = row.vh();

        cubes_[cube->kind()] = cube;
        Put(row.id(), cube);
    }
    return true;
}

}  // namespace res

}  // namespace nyaa