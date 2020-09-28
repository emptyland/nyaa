#pragma once
#ifndef NYAA_SYSTEM_ACTOR_NAVIGATION_SYSTEM_H_
#define NYAA_SYSTEM_ACTOR_NAVIGATION_SYSTEM_H_

#include "system/system.h"

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