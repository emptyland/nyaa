#include "resource/actor-library.h"
#include "resource/avatar-library.h"

namespace nyaa {

namespace res {

const char ActorLibrary::kActorDir[] = "actor";
const char ActorLibrary::kActorDefFileName[] = "actor/def.txt";

bool ActorLibrary::Prepare(const std::string &file_name) {
    (void)arena_;
    (void)avatar_lib_;
    // TODO:
    return false;
}

}  // namespace res

}  // namespace nyaa