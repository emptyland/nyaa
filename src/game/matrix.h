#pragma once
#ifndef NYAA_GAME_MATRIX_H_
#define NYAA_GAME_MATRIX_H_

#include "base/base.h"
#include "glog/logging.h"
#include <math.h>

namespace nyaa {

template <class T = float>
class Matrix {
public:
    static constexpr int kDimX      = 4;
    static constexpr int kDimY      = 4;
    static constexpr int kMaxValues = kDimX * kDimY;

    explicit Matrix(T initial = T(0)) {
        for (int i = 0; i < arraysize(value_); i++) { value_[i] = initial; }
    }

    T *values() { return &value_[0]; }

    T value(int i) const {
        DCHECK_GE(i, 0);
        DCHECK_LT(i, kMaxValues);
        return value_[i];
    }

    T value(int x, int y) const {
        DCHECK_GE(x, 0);
        DCHECK_LT(x, kDimX);
        DCHECK_GE(y, 0);
        DCHECK_LT(y, kDimY);
        return value_[x + y * kDimX];
    }

    void Identity() {
        value_[0]  = 1;
        value_[1]  = 0;
        value_[2]  = 0;
        value_[3]  = 0;
        value_[4]  = 0;
        value_[5]  = 1;
        value_[6]  = 0;
        value_[7]  = 0;
        value_[8]  = 0;
        value_[9]  = 0;
        value_[10] = 1;
        value_[11] = 0;
        value_[12] = 0;
        value_[13] = 0;
        value_[14] = 0;
        value_[15] = 1;
    }

    void Translate(T dx, T dy, T dz) {
        value_[0]  = 1;
        value_[1]  = 0;
        value_[2]  = 0;
        value_[3]  = 0;
        value_[4]  = 0;
        value_[5]  = 1;
        value_[6]  = 0;
        value_[7]  = 0;
        value_[8]  = 0;
        value_[9]  = 0;
        value_[10] = 1;
        value_[11] = 0;
        value_[12] = dx;
        value_[13] = dy;
        value_[14] = dz;
        value_[15] = 1;
    }

    void Rotate(T x, T y, T z, T angle) {
        Normalize(&x, &y, &z);
        T s = sinf(angle);
        T c = cosf(angle);
        T m = 1 - c;

        value_[0]  = m * x * x + c;
        value_[1]  = m * x * y - z * s;
        value_[2]  = m * z * x + y * s;
        value_[3]  = 0;
        value_[4]  = m * x * y + z * s;
        value_[5]  = m * y * y + c;
        value_[6]  = m * y * z - x * s;
        value_[7]  = 0;
        value_[8]  = m * z * x - y * s;
        value_[9]  = m * y * z + x * s;
        value_[10] = m * z * z + c;
        value_[11] = 0;
        value_[12] = 0;
        value_[13] = 0;
        value_[14] = 0;
        value_[15] = 1;
    }

    void Multiply(const Matrix &b) { Multiply(*this, b, this); }

    static void Multiply(const Matrix &a, const Matrix &b, Matrix *matrix) {
        float result[16];
        for (int c = 0; c < kDimX; c++) {
            for (int r = 0; r < kDimY; r++) {
                int   index = c * 4 + r;
                float total = 0;
                for (int i = 0; i < 4; i++) {
                    int p = i * 4 + r;
                    int q = c * 4 + i;
                    total += a.value(p) * b.value(q);
                }
                result[index] = total;
            }
        }
        for (int i = 0; i < 16; i++) { matrix->value_[i] = result[i]; }
    }

    static void Normalize(T *x, T *y, T *z) {
        T d = sqrtf((*x) * (*x) + (*y) * (*y) + (*z) * (*z));
        *x /= d;
        *y /= d;
        *z /= d;
    }

private:
    T value_[kMaxValues];
};  // class Matrix

}  // namespace nyaa

#endif  // NYAA_GAME_MATRIX_H_