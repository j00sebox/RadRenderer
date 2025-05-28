#pragma once

#include "matrix.hpp"
#include "vector.hpp"

#include <algorithm>
#include <cmath>

namespace mathz
{
/**********************************************
class Quaternion
@brief Class that contains the real parts of a
quaternion vector accessed via w, x, y, z.
************************************************/
class Quaternion
{
public:
  // Equivalent of identity matrix
  Quaternion()
      : w(1.f), x(0.f), y(0.f), z(0.f) {}

  Quaternion(float w, float x, float y, float z)
      : w(w), x(x), y(y), z(z)
  {
  }

  // Expects angle to be in radians
  Quaternion(float angle, Vec3 vec)
  {
    w = std::cos(angle * 0.5f);

    float half_angle_sin = std::sin(angle * 0.5f);
    x = vec.x * half_angle_sin;
    y = vec.y * half_angle_sin;
    z = vec.z * half_angle_sin;

    Normalize();
  }

  void Normalize()
  {
    float magnitude = std::sqrt(w * w + x * x + y * y + z * z);
    if (magnitude > 0.f)
    {
      w /= magnitude;
      x /= magnitude;
      y /= magnitude;
      z /= magnitude;
    }
  }

  float Dot(const Quaternion& other) const
  {
    return w * other.w + x * other.x + y * other.y + z * other.z;
  }

  Quaternion Conjugate() const
  {
    return Quaternion(w, -x, -y, -z);
  }

  Mat4 ToMatrix() const
  {
    return Mat4(
        1.f - 2.f * (std::pow(y, 2.f) + std::pow(z, 2.f)), 2.f * (x * y - w * z), 2.f * (x * z + w * y), 0.f,
        2.f * (x * y + w * z), 1.f - 2.f * (std::pow(x, 2.f) + std::pow(z, 2.f)), 2.f * (y * z - w * x), 0.f,
        2.f * (x * z - w * y), 2.f * (y * z + w * x), 1.f - 2.f * (std::pow(x, 2.f) + std::pow(y, 2.f)), 0.f,
        0.f, 0.f, 0.f, 1.f);
  }

  static Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t)
  {
    float dot = q1.Dot(q2);

    // Ensure shortest path
    Quaternion q2_corrected = q2;
    if (dot < 0.0f)
    {
      q2_corrected = q2 * -1.f;
      dot = -dot;
    }

    // If the dot product is close to 1, use linear interpolation to avoid precision issues
    if (dot > 0.9995f)
    {
      return (q1 * (1.f - t) + q2_corrected * t);
    }

    // Clamp to avoid numerical errors
    dot = std::clamp(dot, -1.f, 1.f);
    float theta = std::acos(dot);
    float sin_theta = std::sin(theta);

    if (sin_theta < 1e-6f)
    {
      return q1;
    }

    float w1 = std::sin((1.f - t) * theta) / sin_theta;
    float w2 = std::sin(t * theta) / sin_theta;

    return q1 * w1 + q2_corrected * w2;
  }

  Quaternion operator+(const Quaternion& other) const
  {
    return {w + other.w, x + other.x, y + other.y, z + other.z};
  }

  Quaternion operator*(const Quaternion& other) const
  {
    float new_w = w * other.w - x * other.x - y * other.y - z * other.z;
    float new_x = w * other.x + x * other.w + y * other.z - z * other.y;
    float new_y = w * other.y - x * other.z + y * other.w + z * other.x;
    float new_z = w * other.z + x * other.y - y * other.x + z * other.w;

    return {new_w, new_x, new_y, new_z};
  }

  Quaternion operator*(float scalar) const
  {
    return {w * scalar, x * scalar, y * scalar, z * scalar};
  }

  bool operator==(const Quaternion& other) const
  {
    return (w == other.w &&
            x == other.x &&
            y == other.y &&
            z == other.z);
  }

  float w, x, y, z;
};
} // Namespace mathz
