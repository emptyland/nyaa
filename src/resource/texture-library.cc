#include "resource/texture-library.h"
#include "resource/definition.h"
#include "game/properties.h"
#include "game/game.h"
#include "lodepng.h"
#include <GL/glew.h>

namespace nyaa {

namespace res {

class TextureDef : public Definition<TextureDef> {
public:
    TextureDef() = default;

    DEF_VAL_GETTER(std::string, file_name);
    DEF_VAL_GETTER(ResourceId, id);
    DEF_VAL_GETTER(Vertex4i, bound);
    DEF_VAL_GETTER(int, index);

    void Parse(const std::vector<std::string_view> &items) {
        ParseValue<DefValType::STRING>  (items[0], &file_name_);
        ParseValue<DefValType::ID>      (items[1], &id_);
        ParseValue<DefValType::VERTEX4I>(items[2], &bound_);
        ParseValue<DefValType::I32>     (items[3], &index_);
    }
private:
    std::string file_name_;
    ResourceId id_ = ResourceId::Of(0);
    Vertex4i bound_;
    int index_;
}; // class TextureDef

const char TextureLibrary::kTextureDir[] = "textures";
const char TextureLibrary::kTextureDefFileName[] = "textures/def.txt";

TextureLibrary::~TextureLibrary() {
    // TODO:
}

bool TextureLibrary::Prepare(const std::string &file_name) {
    FILE *fp = ::fopen(file_name.c_str(), "r");
    if (!fp) {
        DLOG(ERROR) << "can not open texture definition file: " << file_name;
        return false;
    }
    std::string last_file_name;
    GLuint last_tex_id = -1;
    Vertex2f last_tex_size;

    DefinitionReader rd(fp, true/*ownership*/);
    TextureDef row;
    while (row.Read(&rd) != EOF) {
        if (row.file_name() != "..") {
            last_file_name = row.file_name();
            if (last_tex_id = LoadPictureFile(last_file_name, &last_tex_size); last_tex_id == -1) {
                return false;
            }
            texture_ids_.push_back(last_tex_id);
        }

        // LT
        Vertex2f p0{row.bound().x/last_tex_size.x, row.bound().y/last_tex_size.y};
        // RT
        Vertex2f p1{(row.bound().x + row.bound().w)/last_tex_size.x, row.bound().y/last_tex_size.y};
        // RB
        Vertex2f p2{(row.bound().x + row.bound().w)/last_tex_size.x, (row.bound().y + row.bound().h)/last_tex_size.y};
        // LB
        Vertex2f p3{row.bound().x/last_tex_size.x, (row.bound().y + row.bound().h)/last_tex_size.y};

        Texture *tex = new (arena_) Texture(row.id(), last_tex_id, row.index(), p0, p1, p2, p3);
        if (textures_.find(row.id()) != textures_.end()) {
            DLOG(ERROR) << "Duplicated tex id: " << row.id().value();
            return false;
        }
        textures_[row.id()] = tex;
    }
    return true;
}

uint32_t TextureLibrary::LoadPictureFile(const std::string &name, Vertex2f *size) {
    std::string full_name(ThisGame->properties()->assets_dir());
    full_name.append("/").append(kTextureDir).append("/").append(name);

    std::vector<Byte> data;
    uint32_t w, h;
    if (auto err = lodepng::decode(data, w, h, full_name); err) {
        DLOG(ERROR) << "Load png file: " << full_name << " fail(" << err << ")";
        return -1;
    }
    size->x = w;
    size->y = h;
    
    GLuint tex_id;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D, 
        0, 
        GL_RGBA, 
        w,
        h, 
        0, 
        GL_RGBA, 
        GL_UNSIGNED_BYTE, 
        &data[0]);
    return tex_id;
}

} // namespace res
    
} // namespace nyaa