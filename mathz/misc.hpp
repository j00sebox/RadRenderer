namespace mathz
{
// Degrees to radians
static constexpr float Radians(float angle)
{
  constexpr float f = 3.14159265f / 180.f;

  return angle * f;
}

// Radians to degrees
static constexpr float Degrees(float angle)
{
  constexpr float f = 180.f / 3.14159265f;

  return angle * f;
}
} // Namespace mathz
