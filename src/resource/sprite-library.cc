#include "resource/sprite-library.h"
#include "resource/definition.h"
#include "resource/texture-library.h"

namespace nyaa {

namespace res {

class SpriteDef : public Definition<SpriteDef> {
public:
    SpriteDef() = default;

    DEF_VAL_GETTER(ResourceId, id);
    DEF_VAL_GETTER(int, frames_count);
    DEF_VAL_GETTER(float, speed);
    DEF_VAL_GETTER(Vector3f, light);
    DEF_VAL_GETTER(Vector3f, ambient_meterial);
    DEF_VAL_GETTER(Vector3f, diffuse_meterial);
    DEF_VAL_GETTER(Vector3f, specular_meterial);

    ResourceId frame(int i) const {
        DCHECK_GE(i, 0);
        DCHECK_LT(i, frames_count());
        return frames_[i];
    }

    void Parse(const std::vector<std::string_view> items) {
        ParseValue<DefValType::ID>(items[0], &id_);
        // ParseValue<DefValType::VECTOR2I>(items[1], &size_);
        ParseValue<DefValType::ARRAY_U32>(items[2], &frames_count_);
        ParseValue<DefValType::F32>(items[3], &speed_);
        // ParseValue<DefValType::ARRAY_U32>(items[4], &down_frames_size_);
        ParseValue<DefValType::VECTOR3F>(items[5], &light_);
        ParseValue<DefValType::VECTOR3F>(items[6], &ambient_meterial_);
        ParseValue<DefValType::VECTOR3F>(items[7], &diffuse_meterial_);
        ParseValue<DefValType::VECTOR3F>(items[8], &specular_meterial_);
    }

private:
    ResourceId       id_;
    std::string_view name_;
    int              frames_count_;
    ResourceId       frames_[Sprite::kMaxFrames];
    float            speed_;
    std::string_view env_;
    Vector3f         light_;
    Vector3f         ambient_meterial_;
    Vector3f         diffuse_meterial_;
    Vector3f         specular_meterial_;
};  // class SpriteDef

const char SpriteLibrary::kSpriteDir[]         = "";
const char SpriteLibrary::kSpriteDefFileName[] = "sprite.txt";

SpriteLibrary::SpriteLibrary(TextureLibrary *tex_lib, base::Arena *arena) : ResourceLibrary(arena), tex_lib_(tex_lib) {}

bool SpriteLibrary::Load(DefinitionReader *rd) {
    SpriteDef row;
    while (row.Read(rd) != EOF) {
        if (FindOrNull(row.id())) {
            DLOG(ERROR) << "Duplicated sprite id: " << row.id().value();
            return false;
        }

        Sprite *sprite = new (arena()) Sprite(row.id(), row.frames_count(), row.speed(), row.light(),
                                             row.ambient_meterial(), row.diffuse_meterial(), row.specular_meterial());
        for (int i = 0; i < row.frames_count(); i++) {
            if (sprite->frames_[i] = tex_lib_->FindOrNull(row.frame(i)); !sprite->frames_[i]) {
                DLOG(ERROR) << "Can not find texture by id: " << row.frame(i).value();
                return false;
            }
        }
        Put(row.id(), sprite);
    }
    return true;
}

}  // namespace res

}  // namespace nyaa
