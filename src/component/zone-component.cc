#include "component/zone-component.h"

namespace nyaa {

namespace com {

ZoneComponent::ZoneComponent() {}

ZoneComponent::~ZoneComponent() {}

CubeComponent *ZoneComponent::Cube(int x, int y, int z) {
    const int lx = x % kRegionSize;
    const int ly = y % kRegionSize;

    DCHECK(region().AtBound(x, y));
    return &mutable_region()->floor(z)->cubes[lx][ly];
}

}  // namespace com

}  // namespace nyaa
