#pragma once

#include <cmath>

constexpr float EPSILON = 1e-5f;

inline bool AlmostEqual(float a, float b, float epsilon = EPSILON)
{
  return std::fabs(a - b) < epsilon;
}
