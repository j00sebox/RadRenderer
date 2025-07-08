#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

#include "../../mathz/quaternion.hpp"
#include "../test_helper.hpp"

bool AlmostEqualQuat(const mathz::Quaternion& a, const mathz::Quaternion& b, float epsilon = EPSILON)
{
  return almostEqual(a.w, b.w, epsilon) &&
         almostEqual(a.x, b.x, epsilon) &&
         almostEqual(a.y, b.y, epsilon) &&
         almostEqual(a.z, b.z, epsilon);
}

TEST_CASE("Default quaternion is identity", "[mathz][Quaternion]")
{
  mathz::Quaternion q;
  REQUIRE(almostEqual(q.w, 1.f));
  REQUIRE(almostEqual(q.x, 0.f));
  REQUIRE(almostEqual(q.y, 0.f));
  REQUIRE(almostEqual(q.z, 0.f));
}

TEST_CASE("Quaternion from angle and axis normalizes", "[mathz][Quaternion]")
{
  mathz::Vec3 axis = {1.f, 0.f, 0.f};
  mathz::Quaternion q(static_cast<float>(PI), axis);
  REQUIRE(almostEqual(q.w, 0.f, 1e-5f));
  REQUIRE(almostEqual(q.x, 1.f, 1e-5f));
  REQUIRE(almostEqual(q.y, 0.f));
  REQUIRE(almostEqual(q.z, 0.f));
}

TEST_CASE("Quaternion dot product is correct", "[mathz][Quaternion]")
{
  mathz::Quaternion a(1.f, 2.f, 3.f, 4.f);
  mathz::Quaternion b(0.5f, -1.f, 2.f, 0.f);
  float dot = a.Dot(b);
  REQUIRE(almostEqual(dot, 1.f * 0.5f + 2.f * -1.f + 3.f * 2.f + 4.f * 0.f));
}

TEST_CASE("Quaternion conjugate inverts xyz", "[mathz][Quaternion]")
{
  mathz::Quaternion q(0.7071f, 0.7071f, 0.f, 0.f);
  mathz::Quaternion c = q.Conjugate();
  REQUIRE(almostEqual(c.w, 0.7071f));
  REQUIRE(almostEqual(c.x, -0.7071f));
  REQUIRE(almostEqual(c.y, 0.f));
  REQUIRE(almostEqual(c.z, 0.f));
}

TEST_CASE("Quaternion additon is correct", "[mathz][Quaternion]")
{
  mathz::Quaternion a(1.f, 2.f, 3.f, 4.f);
  mathz::Quaternion b(5.f, 6.f, 7.f, 8.f);
  mathz::Quaternion sum = a + b;
  REQUIRE(almostEqual(sum.w, 6.f));
  REQUIRE(almostEqual(sum.x, 8.f));
  REQUIRE(almostEqual(sum.y, 10.f));
  REQUIRE(almostEqual(sum.z, 12.f));
}

TEST_CASE("Quaternion multiplied by scalar is correct", "[mathz][Quaternion]")
{
  mathz::Quaternion q(1.f, -2.f, 3.f, -4.f);
  mathz::Quaternion r = q * 2.f;
  REQUIRE(almostEqual(r.w, 2.f));
  REQUIRE(almostEqual(r.x, -4.f));
  REQUIRE(almostEqual(r.y, 6.f));
  REQUIRE(almostEqual(r.z, -8.f));
}

TEST_CASE("Quaternion multiplication is correct", "[mathz][Quaternion]")
{
  mathz::Quaternion a(1.f, 0.f, 1.f, 0.f);
  mathz::Quaternion b(1.f, 0.5f, 0.5f, 0.75f);
  mathz::Quaternion result = a * b;

  REQUIRE(almostEqual(result.w, 0.5f));
  REQUIRE(almostEqual(result.x, 1.25f));
  REQUIRE(almostEqual(result.y, 1.5f));
  REQUIRE(almostEqual(result.z, 0.25f));
}

TEST_CASE("Slerp between identical quaternions returns original", "[mathz][Quaternion]")
{
  mathz::Quaternion a(1.f, 0.f, 0.f, 0.f);
  mathz::Quaternion b(1.f, 0.f, 0.f, 0.f);
  mathz::Quaternion result = mathz::Quaternion::Slerp(a, b, 0.5f);
  REQUIRE(AlmostEqualQuat(result, a));
}

TEST_CASE("Slerp handles opposite quaternions correctly", "[mathz][Quaternion]")
{
  mathz::Quaternion a(0.f, 1.f, 0.f, 0.f);
  mathz::Quaternion b(0.f, -1.f, 0.f, 0.f);
  mathz::Quaternion result = mathz::Quaternion::Slerp(a, b, 0.5f);

  float mag = std::sqrt(result.Dot(result));
  REQUIRE(almostEqual(mag, 1.f));
}
