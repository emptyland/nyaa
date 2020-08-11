#include "resource/cube-library.h"
#include "resource/definition.h"

namespace nyaa {

namespace res {

class CubeDef final : public Definition<CubeDef> {
public:

private:
    
}; // class CubeDef

CubeLibrary::CubeLibrary(TextureLibrary *tex_lib, base::Arena *arena)
    : arena_(arena)
    , tex_lib_(tex_lib)
    , id_to_cubes_(arena) {
    ::memset(cubes_, 0, sizeof(cubes_));
}

bool CubeLibrary::Prepare(const std::string &file_name) {

    return false;
}

} // namespace res
    
} // namespace nyaa