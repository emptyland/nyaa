#pragma once
#ifndef NYAA_SYSTEM_IMPACT_CHECKING_SYSTEM_H_
#define NYAA_SYSTEM_IMPACT_CHECKING_SYSTEM_H_

#include "game/vector.h"
#include "base/base.h"
#include "glog/logging.h"

namespace nyaa {
class EntityGridSet;
namespace com {
class ZoneComponent;
class CubeComponent;
}  // namespace com

namespace sys {

class ImpactCheckingSystem {
public:
    ImpactCheckingSystem(com::ZoneComponent *zone, EntityGridSet *entities) : zone_(zone), entities_(entities) {}

    DEF_PTR_GETTER_NOTNULL(EntityGridSet, entities);
    DEF_PTR_GETTER_NOTNULL(com::ZoneComponent, zone);

    bool PredictTest(const Boundf &body, const Vector3f &next, const Vector3f &dir);

    DISALLOW_IMPLICIT_CONSTRUCTORS(ImpactCheckingSystem);

private:
    const com::CubeComponent *CubeAt(int x, int y, int z);

    com::ZoneComponent *zone_;
    EntityGridSet *     entities_;
};  // class ImpactCheckingSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_SYSTEM_IMPACT_CHECKING_SYSTEM_H_