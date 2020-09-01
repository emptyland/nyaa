#pragma once
#ifndef NYAA_GAME_VECTOR_H_
#define NYAA_GAME_VECTOR_H_

#include <math.h>

namespace nyaa {

template <class T>
struct Vector2 {
    T x;
    T y;
};  // template<class T> struct Vector2

template <class T>
struct Vector3 {
    T x;
    T y;
    T z;
};  // template<class T> struct Vector3

template <class T>
struct Vector4 {
    T x;
    T y;
    T z;
    T w;
};  // // template<class T> struct Vector4

template <class T>
struct Bound {
    T x;
    T y;
    T w;
    T h;
};  // template<class T> struct Bundle2

using Vector2i = Vector2<int>;
using Vector2f = Vector2<float>;

using Vector3i = Vector3<int>;
using Vector3f = Vector3<float>;

using Vector4i = Vector4<int>;
using Vector4f = Vector4<float>;

using Boundi = Bound<int>;
using Boundf = Bound<float>;

inline Vector2f Vec2(float x, float y) { return Vector2f{x, y}; }
inline Vector2f Vec2(const Vector2i &v2i) { return Vec2(v2i.x, v2i.y); }

inline Vector3f Vec3(float x, float y, float z) { return Vector3f{x, y, z}; }
inline Vector3f Vec3(const Vector2f &xy, float z) { return Vector3f{xy.x, xy.y, z}; }
inline Vector3f Vec3(const Vector4f &xyzw) { return Vector3f{xyzw.x, xyzw.y, xyzw.z}; }
inline Vector3f Vec3(const Vector3i &v3i) { return Vec3(v3i.x, v3i.y, v3i.z); }

inline Vector4f Vec4(float x, float y, float z, float w) { return Vector4f{x, y, z, w}; }
inline Vector4f Vec4(const Vector2f &xy, float z, float w) { return Vector4f{xy.x, xy.y, z, w}; }
inline Vector4f Vec4(const Vector3f &xyz, float w) { return Vector4f{xyz.x, xyz.y, xyz.z, w}; }
inline Vector4f Vec4(const Vector4i &v4i) { return Vec4(v4i.x, v4i.y, v4i.z, v4i.w); }

inline Vector2f operator-(const Vector2f &lhs, const Vector2f &rhs) { return Vec2(lhs.x - rhs.x, lhs.y - rhs.y); }
inline Vector2f operator+(const Vector2f &lhs, const Vector2f &rhs) { return Vec2(lhs.x + rhs.x, lhs.y + rhs.y); }

inline Vector3f operator-(const Vector3f &lhs, const Vector3f &rhs) {
    return Vec3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}
inline Vector3f operator+(const Vector3f &lhs, const Vector3f &rhs) {
    return Vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

// a.x, a.y             b.x,b.y
//
//          a.w, a.h            b.w, b.h
/*
    int zx = abs(x01 + x02 -x11 - x12);
	int x  = abs(x01 - x02) + abs(x11 - x12);
	int zy = abs(y01 + y02 - y11 - y12);
	int y  = abs(y01 - y02) + abs(y11 - y12);
	if(zx <= x && zy <= y)
		return 1;
	else
		return 0;

 */
inline int Abs(int n) { return n < 0 ? -n : n; }

inline bool IsIntersect(const Boundf &a, const Boundf &b) {
    int zx = Abs(a.x + (a.x + a.w) - b.x - (b.x + b.w));
    int x = Abs(a.w) + Abs(b.w);
    int zy = Abs(a.y + (a.y + a.h) - b.y - (b.y + b.h));
    int y = Abs(a.h) + Abs(b.h);
    return zx <= x && zy <= y;
}

inline float Square(float n) { return n * n; }

inline float Distance(const Vector2f &a, const Vector2f &b) {
    return ::sqrtf(Square(a.x - b.x) + Square(a.y - b.y));
}

inline float Distance(const Vector3f &a, const Vector3f &b) {
    return ::powf(Square(a.x - b.x) + Square(a.y - b.y) + Square(a.z - b.z), 1.0/3.0);
}

}  // namespace nyaa

#endif  // NYAA_GAME_VECTOR_H_