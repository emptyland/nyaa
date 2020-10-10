#include "system/world-generating-system.h"
#include "component/zone-component.h"
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

    struct FillContext {
        PseudoRandomGenerator *      random;
        com::RegionComponent::Floor *floor;
        com::RegionComponent::Floor *lower;
        struct {
            res::Cube::Kind kind;
            float           ratio;
        } cubes[8];
    };  // struct FillContext

    static void GeneratePlain(PseudoRandomGenerator *random, com::ZoneComponent *zone) {
        FillContext fill;
        fill.floor    = zone->mutable_region()->floor(0);
        fill.lower    = nullptr;
        fill.random   = random;
        fill.cubes[0] = {res::Cube::CUBE_DIRT_1, 0.2f};
        fill.cubes[1] = {res::Cube::CUBE_DIRT_2, 0.4f};
        fill.cubes[2] = {res::Cube::CUBE_GRASS_1, 0.6f};
        fill.cubes[3] = {res::Cube::CUBE_GRASS_2, 0.8f};
        fill.cubes[4] = {res::Cube::CUBE_STONE_1, 1.0f};
        FillFloor(&fill);

        fill.lower    = fill.floor;
        fill.floor    = zone->mutable_region()->floor(1);
        fill.cubes[0] = {res::Cube::CUBE_DIRT_1, 0.2f};
        fill.cubes[1] = {res::Cube::CUBE_STONE_1, 0.4f};
        fill.cubes[2] = {res::Cube::CUBE_AIR, 1.0f};
        FillFloor(&fill);
        SmoothFloorManyTimes(&fill, 4);

        fill.lower = fill.floor;
        fill.floor = zone->mutable_region()->floor(2);
        FillFloor(&fill);
        SmoothFloorManyTimes(&fill, 4);
    }

private:
    static void FillFloor(FillContext *ctx) {
        for (int y = 0; y < kRegionSize; y++) {
            for (int x = 0; x < kRegionSize; x++) {
                com::CubeComponent *cube = &ctx->floor->cubes[x][y];

                float factor = ctx->random->NextFactor();
                for (int i = 0; i < arraysize(ctx->cubes); i++) {
                    if (factor < ctx->cubes[i].ratio) {
                        cube->set_kind(ctx->cubes[i].kind);
                        if (!cube->IsAir()) { cube->set_hardness(4); }
                    }
                }
            }
        }
    }

    static void SmoothFloorManyTimes(FillContext *ctx, int n) {
        for (int i = 0; i < n; i++) { SmoothFloor(ctx); }
    }

    static void SmoothFloor(FillContext *ctx) {
        for (int y = 1; y < kRegionSize - 1; y++) {
            for (int x = 1; x < kRegionSize - 1; x++) {
                com::CubeComponent *cube = &ctx->floor->cubes[x][y];
                if (cube->IsAir() || ctx->lower->cubes[x][y].IsAir()) { continue; }

                if (ShouldCubeOpen(ctx->floor, x, y)) {
                    cube->set_kind(res::Cube::CUBE_AIR);
                    cube->set_hardness(0);
                    continue;
                }

                float factor = ctx->random->NextFactor();
                for (int i = 0; i < arraysize(ctx->cubes); i++) {
                    if (factor < ctx->cubes[i].ratio) {
                        cube->set_kind(ctx->cubes[i].kind);
                        if (!cube->IsAir()) { cube->set_hardness(4); }
                    }
                }
            }
        }
    }

    static bool ShouldCubeOpen(com::RegionComponent::Floor *floor, int i, int j) {
        return CountSurroundCubes(floor, i, j) < 4;
    }

    static int CountSurroundCubes(com::RegionComponent::Floor *floor, int i, int j) {
        int count = 0;
        for (int y = j - 1; y < j + 1; y++) {
            for (int x = i - 1; x < i + 1; x++) {
                if (!floor->cubes[x][y].IsAir()) { count++; }
            }
        }
        return count;
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
    PseudoRandomGenerator random(seed);
    switch (env) {
        case ZoneEnv::kPlain: Core::GeneratePlain(&random, zone); break;
        case ZoneEnv::kForest: break;
        case ZoneEnv::kSnowfield: break;
        case ZoneEnv::kMountain: break;
        default: NOREACHED(); break;
    }
}

}  // namespace sys

}  // namespace nyaa
