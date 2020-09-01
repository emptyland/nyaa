#pragma once
#ifndef NYAA_RESOURCE_AI_DEF_H_
#define NYAA_RESOURCE_AI_DEF_H_

#include "base/base.h"
#include <string_view>

namespace nyaa {

namespace res {

#define DECLARE_AI_KINDS(V) \
    V(NONE)                 \
    V(STUPID)               \
    V(HAND_OFF)             \
    V(FOLLOW)

struct AI {
    enum Kind {
#define DEFINE_ENUM(name) name,
        DECLARE_AI_KINDS(DEFINE_ENUM)
#undef DEFINE_ENUM
            MAX_AI_KIND,
    };  // enum Kind

    static Kind        OfName(std::string_view name);
    static const char *Name(Kind kind);

    DISALLOW_ALL_CONSTRUCTORS(AI);
};  // struct AI

}  // namespace res

}  // namespace nyaa

#endif  // NYAA_RESOURCE_AI_DEF_H_