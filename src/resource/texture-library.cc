#include "resource/texture-library.h"
#include "resource/definition.h"

namespace nyaa {

namespace res {

class TextureDef : public Definition<TextureLibrary> {
public:
    TextureDef() = default;

    DEF_VAL_GETTER(std::string, file_name);
    DEF_VAL_GETTER(ResourceId, id);
    DEF_VAL_GETTER(Vertex4i, bound);
    DEF_VAL_GETTER(int, index);

    void Parse(const std::vector<std::string_view> &items) {
        ParseValue(items[0], DefValType::STRING, &file_name_);
        ParseValue(items[1], DefValType::ID, &id_);
        ParseValue(items[2], DefValType::VERTEX4I, &bound_);
        ParseValue(items[3], DefValType::I32, &index_);
    }
private:
    std::string file_name_;
    ResourceId id_ = ResourceId::Of(0);
    Vertex4i bound_;
    int index_;
}; // class TextureDef

TextureLibrary::~TextureLibrary() {
    // TODO:
}

bool TextureLibrary::Prepare(const std::string &file_name) {
    FILE *fp = ::fopen(file_name.c_str(), "r");
    if (!fp) {
        DLOG(ERROR) << "can not open texture definition file: " << file_name;
        return false;
    }
    DefinitionReader rd(fp, true/*ownership*/);
    TextureDef row;
    while (row.Read(&rd) != EOF) {
        // TODO:
    }
    // TODO;
    return false;
}

} // namespace res
    
} // namespace nyaa