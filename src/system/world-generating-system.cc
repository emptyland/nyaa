#include "system/world-generating-system.h"
#include "game/pseudo-random-number.h"
#include "game/properties.h"
#include "game/world-map.h"
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

#define FILE_OP_DO(cond)                       \
    if (cond) {                                \
        PLOG(ERROR) << "File operation fail."; \
        *err = strerror(errno);                \
        return;                                \
    }                                          \
    (void)0

    static void ReadWorldIndex(FILE *fp, WorldMap *map, std::string *err) {
        Vector2i size = ReadWorldSize(fp, err);
        if (!err->empty()) { return; }
        DCHECK_EQ(size.x, map->size().x);
        DCHECK_EQ(size.y, map->size().y);

        long pos = ::ftell(fp);
        FILE_OP_DO(pos < 0);
        FILE_OP_DO(::fseek(fp, 0, SEEK_END) != 0);
        long end = ::ftell(fp);
        FILE_OP_DO(pos < 0);
        std::string buf;
        buf.resize(end - pos);
        FILE_OP_DO(::fseek(fp, pos, SEEK_SET) != 0);
        FILE_OP_DO(::fread(&buf[0], 1, buf.size(), fp) <= 0);

        base::BufferReader rd(buf);
        for (int j = 0; j < size.y; j++) {
            for (int i = 0; i < size.x; i++) {
                WorldMap::Index *index = map->get(i, j);

                index->coord.x = rd.ReadVarint32();
                index->coord.y = rd.ReadVarint32();
                index->seed    = rd.ReadFixed64();
                index->env     = static_cast<ZoneEnv::Kind>(rd.ReadVarint32());
            }
        }
    }

    static Vector2i ReadWorldSize(FILE *fp, std::string *err) {
        char header[Files::kWorldIndexHeaderSize] = {0};
        ::fread(header, 1, arraysize(header), fp);
        if (::strncmp(Files::kWorldIndexMagic, header, sizeof(Files::kWorldIndexMagic) - 1) != 0) {
            *err = "Bad index file!";
            return {0, 0};
        }

        char buf[22];
        ::fread(buf, 1, arraysize(buf), fp);
        base::BufferReader rd(std::string_view{buf, 22});
        if (uint32_t ver = rd.ReadFixed32(); ver > Files::kVersion) {
            *err = base::Sprintf("Invalid version: %08x", ver);
            return {0, 0};
        }

        Vector2i size;
        rd.ReadFixed64();
        size.x = rd.ReadVarint32();
        size.y = rd.ReadVarint32();
        ::fseek(fp, static_cast<long>(rd.position()) - static_cast<long>(arraysize(buf)), SEEK_SET);
        return size;
    }

    static FILE *OpenWorldIndexFile(std::string_view world_id, std::string *err) {
        fs::path path = Files::WorldIndexPath(world_id);
        if (!fs::exists(path)) {
            *err = "World index file: " + path.string() + " not found.";
            return nullptr;
        }

        FILE *fp = ::fopen(path.string().c_str(), "rb");
        if (!fp) {
            PLOG(ERROR) << "Open file: " << path << " fail!";
            *err = strerror(errno);
            return nullptr;
        }
        return fp;
    }
};  // class ZoneGeneratingSystem::Core

std::string WorldGeneratingSystem::InitWorldMap(std::string_view name, PseudoRandomGenerator *random,
                                                WorldMapDef::Size size, std::string *err) {
    fs::path path;
    path.append(Game::This()->properties()->data_dir()).append(Files::kWorldListName);
    const char *mod = fs::exists(path) ? "ab" : "wb";

    FILE *fp = ::fopen(path.string().c_str(), mod);
    if (!fp) {
        PLOG(ERROR) << "Open file: " << path << " fail!";
        *err = strerror(errno);
        return "";
    }

    std::string world_id = base::Sprintf("%08x", Game::This()->random()->NextI32());
    ::fprintf(fp, "%s:%.*s\n", world_id.c_str(), static_cast<int>(name.size()), name.data());
    ::fclose(fp);

    path = Files::WorldPath(world_id);
    if (!fs::create_directories(path)) {
        PLOG(ERROR) << "Mkdir: " << path << " fail!";
        *err = strerror(errno);
        return "";
    }

    path.append(Files::kWorldIndexName);
    if (fp = ::fopen(path.string().c_str(), "wb"); !fp) {
        PLOG(ERROR) << "Open file: " << path << " fail!";
        *err = strerror(errno);
        return "";
    }
    Core::GenerateWorldIndex(fp, world_id, random, size);
    ::fclose(fp);
    return world_id;
}

Vector2i WorldGeneratingSystem::ReadWorldSize(std::string_view world_id, std::string *err) {
    FILE *fp = Core::OpenWorldIndexFile(world_id, err);
    if (!fp) { return {0, 0}; }
    Vector2i size = Core::ReadWorldSize(fp, err);
    ::fclose(fp);
    return size;
}

void WorldGeneratingSystem::ReadWorldIndex(std::string_view world_id, WorldMap *map, std::string *err) {
    FILE *fp = Core::OpenWorldIndexFile(world_id, err);
    if (!fp) { return; }
    Core::ReadWorldIndex(fp, map, err);
    ::fclose(fp);
}

void WorldGeneratingSystem::Generate(uint64_t seed, ZoneEnv::Kind env, com::ZoneComponent *zone) {
    // TODO:
    // random->NextFactor();
}

}  // namespace sys

}  // namespace nyaa
