#include "resource/avatar-library.h"
#include "resource/texture-library.h"
#include "resource/definition.h"
#include "game/properties.h"
#include "game/game.h"

namespace nyaa {

namespace res {

class AvatarDef final : public Definition<AvatarDef> {
public:
    AvatarDef() = default;

    DEF_VAL_GETTER(ResourceId, id);
    DEF_VAL_GETTER(Vertex2i, size);
    DEF_VAL_GETTER(float, speed);
    DEF_VAL_GETTER(Vertex4i, key_frames);
    DEF_VAL_GETTER(int, up_frames_size);
    DEF_VAL_GETTER(int, right_frames_size);
    DEF_VAL_GETTER(int, down_frames_size);
    DEF_VAL_GETTER(int, left_frames_size);
    ResourceId *up_frames() { return up_frames_; }
    ResourceId *right_frames() { return right_frames_; }
    ResourceId *down_frames() { return down_frames_; }
    ResourceId *left_frames() { return left_frames_; }

    void Parse(const std::vector<std::string_view> items) {
        ParseValue<DefValType::ID>       (items[0], &id_);
        ParseValue<DefValType::VERTEX2I> (items[1], &size_);
        ParseValue<DefValType::ARRAY_U32>(items[2], &up_frames_size_);
        ParseValue<DefValType::ARRAY_U32>(items[3], &right_frames_size_);
        ParseValue<DefValType::ARRAY_U32>(items[4], &down_frames_size_);
        ParseValue<DefValType::ARRAY_U32>(items[5], &left_frames_size_);
        ParseValue<DefValType::F32>      (items[6], &speed_);
        ParseValue<DefValType::VERTEX4I> (items[7], &key_frames_);
    }

private:
    ResourceId id_;
    Vertex2i size_;
    int up_frames_size_;
    ResourceId up_frames_[Avatar::kMaxFrames];
    int right_frames_size_;
    ResourceId right_frames_[Avatar::kMaxFrames];
    int down_frames_size_;
    ResourceId down_frames_[Avatar::kMaxFrames];
    int left_frames_size_;
    ResourceId left_frames_[Avatar::kMaxFrames];
    float speed_;
    Vertex4i key_frames_;
}; // class AvatarDef

Avatar::Avatar(ResourceId id, Vertex2f size, float speed, int frames_count)
    : id_(id)
    , size_(size)
    , speed_(speed)
    , frames_count_(frames_count) {
    ::memset(key_frame_, 0, sizeof(key_frame_));
    ::memset(textures_, 0, sizeof(textures_));
}

bool AvatarLibrary::Prepare(const std::string &file_name) {
    FILE *fp = ::fopen(file_name.c_str(), "r");
    if (!fp) {
        DLOG(ERROR) << "can not open texture definition file: " << file_name;
        return false;
    }

    DefinitionReader rd(fp, true/*ownership*/);
    AvatarDef row;
    while (row.Read(&rd) != EOF) {
        if (avatars_.find(row.id()) != avatars_.end()) {
            DLOG(ERROR) << "Duplicated avatar id: " << row.id().value();
            return false;
        }

        int frame_count = std::max(row.up_frames_size(), row.down_frames_size());
        frame_count = std::max(frame_count, row.left_frames_size());
        frame_count = std::max(frame_count, row.right_frames_size());
        Vertex2f size {
            static_cast<float>(row.size().x),
            static_cast<float>(row.size().y),
        };
        Avatar *avatar = new (arena_) Avatar(row.id(), size, row.speed(), frame_count);
        avatar->key_frame_[Avatar::kUp]    = row.key_frames().x;
        avatar->key_frame_[Avatar::kRight] = row.key_frames().y;
        avatar->key_frame_[Avatar::kDown]  = row.key_frames().w;
        avatar->key_frame_[Avatar::kLeft]  = row.key_frames().h;

        Texture *tex;
        for (int i = 0; i < row.up_frames_size(); i++) {
            if (tex = tex_lib_->FindOrNull(row.up_frames()[i]); !tex) {
                // TODO:
                return false;
            }
            avatar->textures_[Avatar::kUp][i] = tex;
        }
        // TODO:
    }
    return true;
}

} // namespace res

} // namespace nyaa