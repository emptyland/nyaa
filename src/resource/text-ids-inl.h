#pragma once
#ifndef NYAA_RESOURCE_TEXT_IDS_INL_H_
#define NYAA_RESOURCE_TEXT_IDS_INL_H_

namespace nyaa {
namespace res {

#define DEFINE_TEXT_KINDS(V) \
    V(TEST_STRING_1) \
    V(TEST_STRING_2) \
    V(TITLE_OPTION_1) \
    V(TITLE_OPTION_2) \
    V(TITLE_OPTION_3) \
    V(TITLE_ROLE_1) \
    V(TITLE_ROLE_2) \
    V(TITLE_ROLE_3) \
    V(TITLE_ROLE_4) \
    V(TITLE_ROLE_5) \
    V(CUBE_AIR) \
    V(CUBE_WALL_1) \
    V(CUBE_WOOD_1) \
    V(NICK_ONE_LINE_MAN) \
    V(ACTOR_JACK_LI) \
    V(ACTOR_NIKO_CHEN) \
    V(ACTOR_STEVE_WANG) \
    V(ACTOR_TOMMY_ZHANG) \
    V(ACTOR_TIMMY_ZHANG) \


enum TextID {
#define DEFINE_ENUM(name) name,
    DEFINE_TEXT_KINDS(DEFINE_ENUM)
#undef DEFINE_ENUM
    MAX_TEXT_ID,
};  // enum TextID

} // namespace res
} // namespace nyaa

#endif // NYAA_RESOURCE_TEXT_IDS_INL_H_
