#pragma once

#include <iostream>
#include <math.h>

namespace mathz
{

/**********************************************
class Vec2
@brief Class used to represent a vector in 2D
                space.
************************************************/
template <typename T>
class Vec2
{
public:
  Vec2() : x(0), y(0) {}

  Vec2(T _x, T _y) : x(_x), y(_y) {}

  Vec2(std::pair<T, T> pair)
  {
    x = pair.first;
    y = pair.second;
  }

  float cross(Vec2& line)
  {
    return (x * line.y) - (y * line.x);
  }

  float dot(Vec2& vec)
  {
    return x * vec.x + y * vec.y;
  }

  void normalize()
  {
    float normalize = sqrtf(pow(x, 2) + pow(y, 2));

    x /= normalize;
    y /= normalize;
  }

  void subtract(Vec2& inputVec, Vec2& outVec)
  {
    outVec.x = x - inputVec.x;
    outVec.y = y - inputVec.y;
  }

  void scalarMultiply(Vec2& outVec, float scalar)
  {
    outVec.x = scalar * x;
    outVec.y = scalar * y;
  }

  void add(Vec2& a, Vec2& outVec)
  {
    outVec.x = x + a.x;
    outVec.y = y + a.y;
  }

  inline Vec2 operator+(const Vec2& v) const
  {
      Vec2 r;

      r.x = x + v.x;
      r.y = y + v.y;
      
      return r;
  }

  inline Vec2 operator*(float scalar) const
  {
      Vec2 r;

      r.x = x * scalar;
      r.y = y * scalar;

      return r;
  }

  inline Vec2 operator/(float scalar) const
  {
      if (scalar == 0.f)
          return {};

      Vec2 r;

      r.x = x / scalar;
      r.y = y / scalar;
  }

  T x, y;
};

/**********************************************
class Vec3
@brief Class used to represent a vector in 3D
                space with float values.
************************************************/
class Vec3
{
public:
  Vec3() : x(0), y(0), z(0) {}

  Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

  inline Vec3 cross(const Vec3& line) const
  {
    Vec3 normal;

    normal.x = y * line.z - z * line.y;
    normal.y = z * line.x - x * line.z;
    normal.z = x * line.y - y * line.x;

    return normal;
  }

  inline float dot(const Vec3& other_vector) const
  {
    return x * other_vector.x + y * other_vector.y + z * other_vector.z;
  }

  inline void normalize()
  {
    float divisor = sqrtf(powf(x, 2.f) + powf(y, 2.f) + powf(z, 2.f));

    if (divisor > 0.f)
    {
      x /= divisor;
      y /= divisor;
      z /= divisor;
    }
  }

  inline Vec3 operator+(const Vec3& v) const
  {
    Vec3 r;

    r.x = x + v.x;
    r.y = y + v.y;
    r.z = z + v.z;

    return r;
  }

  inline Vec3 operator-(const Vec3& v) const
  {
    Vec3 r;

    r.x = x - v.x;
    r.y = y - v.y;
    r.z = z - v.z;

    return r;
  }

  inline Vec3 operator*(float scalar) const
  {
    Vec3 r;

    r.x = x * scalar;
    r.y = y * scalar;
    r.z = z * scalar;

    return r;
  }

  friend std::ostream& operator<<(std::ostream& os, const Vec3& vector)
  {
    os << "[ " << vector.x << " " << vector.y << " " << vector.z << " ]\n";
    return os;
  }

  float x, y, z;
};

/**********************************************
class Vec4
@brief Class used to represent a vector in 4D
                space with float values.
************************************************/
class Vec4
{
public:
  Vec4() : x(0), y(0), z(0), w(1.f) {}

  Vec4(const Vec3& vec)
  {
    x = vec.x;
    y = vec.y;
    z = vec.z;
    w = 1.f;
  }

  Vec4(float x, float y, float z, float w)
  {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
  }

  inline void normalize()
  {
    float divisor = sqrtf(powf(x, 2.f) + powf(y, 2.f) + powf(z, 2.f) + powf(w, 2.f));

    if (divisor > 0.f)
    {
      x /= divisor;
      y /= divisor;
      z /= divisor;
      w /= divisor;
    }
  }

  inline Vec4 operator+(const Vec4& v)
  {
    Vec4 r;

    r.x = x + v.x;
    r.y = y + v.y;
    r.z = z + v.z;
    r.w = w + v.w;

    return r;
  }

  inline Vec4 operator-(const Vec4& v)
  {
    Vec4 r;

    r.x = x - v.x;
    r.y = y - v.y;
    r.z = z - v.z;
    r.w = w - v.w;

    return r;
  }

  inline Vec4 operator*(float a) const
  {
    Vec4 r;

    r.x = x * a;
    r.y = y * a;
    r.z = z * a;
    r.w = w * a;

    return r;
  }

  inline bool operator==(const Vec4& other) const
  {
    return ((x == other.x) &&
            (y == other.y) &&
            (z == other.z) &&
            (w == other.w));
  }

  float x, y, z, w;
};
} // Namespace mathz
