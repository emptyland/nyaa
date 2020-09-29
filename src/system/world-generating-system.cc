#include "system/world-generating-system.h"
#include "game/pseudo-random-number.h"
#include "game/properties.h"
#include "game/game.h"
#include <filesystem>

namespace nyaa {

namespace sys {

class WorldGeneratingSystem::Core {
public:
    static void GenerateWorldIndex(FILE *fp, std::string_view id, PseudoRandomGenerator *random,
                                   WorldMapDef::Size kind) {
        Vector2i size = WorldMapDef::SizeOf(kind);
        ::fprintf(fp, "world:%s\n", id.data());
        std::string buf;
        base::Slice::WriteFixed32(&buf, Files::kVersion);
        base::Slice::WriteFixed64(&buf, random->seed());
        base::Slice::WriteVarint32(&buf, size.x);
        base::Slice::WriteVarint32(&buf, size.y);
        ::fwrite(buf.data(), 1, buf.size(), fp);

        buf.clear();
        for (int j = 0; j < size.y; j++) {
            random->Chaosize();
            for (int i = 0; i < size.x; i++) {
                base::Slice::WriteVarint32(&buf, i);
                base::Slice::WriteVarint32(&buf, j);
                base::Slice::WriteFixed64(&buf, random->Next());
                base::Slice::WriteVarint32(&buf, ZoneEnv::Choose(random->NextFactor())->env);
            }
        }
        ::fwrite(buf.data(), 1, buf.size(), fp);
    }

private:
};  // class ZoneGeneratingSystem::Core

bool WorldGeneratingSystem::InitWorldMap(PseudoRandomGenerator *random, WorldMapDef::Size size, std::string *err) {
    fs::path path(Game::This()->properties()->data_dir());
    path.append("/").append(Files::kWorldListName);

    const char *mod = fs::exists(path) ? "ab" : "wb";
    FILE *      fp  = ::fopen(path.generic_u8string().c_str(), mod);
    if (!fp) {
        PLOG(ERROR) << "Open file: " << path << " fail!";
        *err = strerror(errno);
        return false;
    }

    std::string world_id = base::Sprintf("%08x", Game::This()->random()->NextI32());
    ::fprintf(fp, "%s\n", world_id.c_str());
    ::fclose(fp);

    path = Files::WorldPath(world_id);
    if (!fs::create_directories(path)) {
        PLOG(ERROR) << "Mkdir: " << path << " fail!";
        *err = strerror(errno);
        return false;
    }

    path.append("/").append(Files::kWorldIndexName);
    if (fp = ::fopen(path.generic_u8string().c_str(), "wb"); !fp) {
        PLOG(ERROR) << "Open file: " << path << " fail!";
        *err = strerror(errno);
        return false;
    }
    Core::GenerateWorldIndex(fp, world_id, random, size);
    ::fclose(fp);
    return true;
}

void WorldGeneratingSystem::Generate(uint64_t seed, ZoneEnv::Kind env, com::ZoneComponent *zone) {

    // random->NextFactor();
}

}  // namespace sys

}  // namespace nyaa
