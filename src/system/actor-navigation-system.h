#pragma once
#ifndef NYAA_SYSTEM_ACTOR_NAVIGATION_SYSTEM_H_
#define NYAA_SYSTEM_ACTOR_NAVIGATION_SYSTEM_H_

#include "base/base.h"

namespace nyaa {

namespace sys {

class ActorNavigationSystem {
public:
    ActorNavigationSystem() = default;

    void Update();

    DISALLOW_IMPLICIT_CONSTRUCTORS(ActorNavigationSystem);

private:
};  // class ActorNavigationSystem

}  // namespace sys

}  // namespace nyaa

#endif  // NYAA_SYSTEM_ACTOR_NAVIGATION_SYSTEM_H_