#include "system/impact-checking-system.h"
#include "component/zone-component.h"
#include "component/cube-component.h"
#include "game/entity-grid-set.h"

namespace nyaa {

namespace sys {

bool ImpactCheckingSystem::PredictTest(const Boundf &body, const Vector3f &next, const Vector3f &dir) {
    bool stop = false;
    if (dir.x != 0) {
        float d = dir.x < 0 ? -1 : +1;
        for (int i = -2; i < 1 && !stop; i++) {
            float x = static_cast<int>(next.x + d);
            float y = static_cast<int>(next.y + i);
            if (const com::CubeComponent *cube = CubeAt(x, y, ceil(next.z) + 1); !cube->IsAir()) {
                stop = IsIntersect(body, {x, y, 1, 1});
            }
        }
    }

    if (dir.y != 0) {
        float d = dir.y < 0 ? -0.2 : +0.2;
        for (int i = -1; i < 2 && !stop; i++) {
            float x = static_cast<int>(next.x + i);
            float y = static_cast<int>(next.y + d);
            if (const com::CubeComponent *cube = CubeAt(x, y, ceil(next.z) + 1); !cube->IsAir()) {
                stop = IsIntersect(body, {x, y, -1, 1});
            }
        }
    }

    return stop;
}

const com::CubeComponent *ImpactCheckingSystem::CubeAt(int x, int y, int z) {
    static com::CubeComponent stub;
    stub.set_kind(res::Cube::CUBE_TREE_STUB);

    if (x < 0 || x >= kRegionSize || y < 0 || y >= kRegionSize || z < 0 || z >= kTerrainMaxLevels) {
        return &stub;
    }
    return zone_->Cube(x, y, z);
}

}  // namespace sys

}  // namespace nyaa
