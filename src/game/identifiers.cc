#include "game/identifiers.h"
#include "game/game.h"

namespace nyaa {

/*static*/ EntityId EntityId::Next() { return ThisGame->NextEntityId(); }

/*static*/ EntityId EntityId::New() { return ThisGame->NewEntityId(); }

}  // namespace nyaa
