#pragma once
#ifndef NYAA_COMPONENT_AI_STATE_COMPONENT_H_
#define NYAA_COMPONENT_AI_STATE_COMPONENT_H_

#include "resource/ai-def.h"
#include "game/identifiers.h"
#include "game/vector.h"

namespace nyaa {

namespace com {

class AIStateComponent {
public:
    static constexpr int kMaxHatredEntities = 8;

    AIStateComponent(res::AI::Kind kind) : kind_(kind) { ::memset(hatred_, 0, sizeof(hatred_)); }

    DEF_VAL_PROP_RW(res::AI::Kind, kind);
    DEF_VAL_PROP_RMW(Vector3f, next_dest);
    DEF_VAL_GETTER(int, hatred_count);

    EntityId hatred(int i) const {
        DCHECK_GE(i, 0);
        DCHECK_LT(i, hatred_count());
        return hatred_[i];
    }

private:
    res::AI::Kind kind_;
    Vector3f      next_dest_ = {0, 0, 0};
    int           hatred_count_ = 0;
    EntityId      hatred_[kMaxHatredEntities];
};  // class AIStateComponent

}  // namespace com

}  // namespace nyaa

#endif  // NYAA_COMPONENT_AI_STATE_COMPONENT_H_