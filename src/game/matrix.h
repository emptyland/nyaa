#pragma once
#ifndef NYAA_GAME_MATRIX_H_
#define NYAA_GAME_MATRIX_H_

#include "game/vector.h"
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

    T const *values() const { return &value_[0]; }

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
        T s = sinf(angle * M_PI / 180.0);
        T c = cosf(angle * M_PI / 180.0);
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

    void Scale(T sx, T sy, T sz) {
        value_[0]  = sx;
        value_[1]  = 0;
        value_[2]  = 0;
        value_[3]  = 0;
        value_[4]  = 0;
        value_[5]  = sy;
        value_[6]  = 0;
        value_[7]  = 0;
        value_[8]  = 0;
        value_[9]  = 0;
        value_[10] = sz;
        value_[11] = 0;
        value_[12] = 0;
        value_[13] = 0;
        value_[14] = 0;
        value_[15] = 1;
    }

    void Frustum(T left, T right, T bottom, T top, T znear, T zfar) {
        T temp  = 2.0 * znear;
        T temp2 = right - left;
        T temp3 = top - bottom;
        T temp4 = zfar - znear;

        value_[0]  = temp / temp2;
        value_[1]  = 0.0;
        value_[2]  = 0.0;
        value_[3]  = 0.0;
        value_[4]  = 0.0;
        value_[5]  = temp / temp3;
        value_[6]  = 0.0;
        value_[7]  = 0.0;
        value_[8]  = (right + left) / temp2;
        value_[9]  = (top + bottom) / temp3;
        value_[10] = (-zfar - znear) / temp4;
        value_[11] = -1.0;
        value_[12] = 0.0;
        value_[13] = 0.0;
        value_[14] = (-temp * zfar) / temp4;
        value_[15] = 0;
    }

    void Perspective(T fov, T aspect, T znear, T zfar) {
        T ymax = znear * tanf(fov * M_PI / 360.0);
        T xmax = ymax * aspect;
        Frustum(-xmax, xmax, -ymax, ymax, znear, zfar);
    }

    void Ortho(T left, T right, T bottom, T top, T near, T far) {
        value_[0]  = 2 / (right - left);
        value_[1]  = 0;
        value_[2]  = 0;
        value_[3]  = 0;
        value_[4]  = 0;
        value_[5]  = 2 / (top - bottom);
        value_[6]  = 0;
        value_[7]  = 0;
        value_[8]  = 0;
        value_[9]  = 0;
        value_[10] = -2 / (far - near);
        value_[11] = 0;
        value_[12] = -(right + left) / (right - left);
        value_[13] = -(top + bottom) / (top - bottom);
        value_[14] = -(far + near) / (far - near);
        value_[15] = 1;
    }

    void Multiply(const Matrix &b) { Multiply(*this, b, this); }

    // a[1] * b[1][1] + a[2] * b[2][1] + a[3] * b[3][1] + a[4] * b[4][1]
    // a[1] * b[1][2] + a[2] * b[2][2] + a[3] * b[3][2] + a[4] * b[4][2]
    // a[1] * b[1][3] + a[2] * b[2][3] + a[3] * b[3][3] + a[4] * b[4][3]
    // a[1] * b[1][4] + a[2] * b[2][4] + a[3] * b[3][4] + a[4] * b[4][4]
    static void Multiply(const Vector4<T> &a, const Matrix &b, Vector4<T> *vec) {
        T result[4];

        T const *f = &a.x;
        for (int c = 0; c < kDimX; c++) {
            T total = 0;
            for (int i = 0; i < 4; i++) { total += f[i] * b.value(i, c); }
            result[c] = total;
        }
        vec->x = result[0];
        vec->y = result[1];
        vec->z = result[2];
        vec->w = result[3];
    }

    static void Multiply(const Matrix &a, const Matrix &b, Matrix *matrix) {
        T result[16];
        for (int c = 0; c < kDimX; c++) {
            for (int r = 0; r < kDimY; r++) {
                int index = c * 4 + r;
                T   total = 0;
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