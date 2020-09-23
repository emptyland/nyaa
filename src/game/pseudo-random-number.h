#pragma once
#ifndef NYAA_GAME_PSEUDO_RANDOM_NUMBER_H_
#define NYAA_GAME_PSEUDO_RANDOM_NUMBER_H_

#include "base/base.h"
#include <stdlib.h>
#include <limits>

namespace nyaa {

class PseudoRandomGenerator {
public:
    PseudoRandomGenerator(int64_t seed) : seed_(seed), n_(seed) {}

    DEF_VAL_GETTER(int64_t, seed);
    DEF_VAL_GETTER(int64_t, n);

    void ResetSeed(int64_t seed) {
        seed_ = seed;
        n_    = seed;
    }

    int64_t Next() {
        n_ = ((n_ * 13) + 10003277) % 0xf123456798abcdefULL;
        return n_;
    }

    int32_t NextI32() {
        Next();
        return static_cast<int32_t>(n() & 0xffffffff);
    }

    float NextFactor() {
        float val = static_cast<float>(Next() & 0xffff);
        return val / 65535.0f;
    }

    void Chaosize() { n_ = (n_ * 7 + 10001707) % 0xf123456798abcdefULL; }

private:
    int64_t seed_ = 0;
    int64_t n_    = 0;
};

}  // namespace nyaa

#endif  // NYAA_GAME_PSEUDO_RANDOM_NUMBER_H_