#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../../mathz/quaternion.hpp"
#include "../test_helper.hpp"

bool AlmostEqualQuat(const mathz::Quaternion& a, const mathz::Quaternion& b, float epsilon = EPSILON)
{
  return AlmostEqual(a.w, b.w, epsilon) &&
         AlmostEqual(a.x, b.x, epsilon) &&
         AlmostEqual(a.y, b.y, epsilon) &&
         AlmostEqual(a.z, b.z, epsilon);
}

TEST_CASE("Default quaternion is identity", "[mathz][Quaternion]")
{
  mathz::Quaternion q;
  REQUIRE(AlmostEqual(q.w, 1.f));
  REQUIRE(AlmostEqual(q.x, 0.f));
  REQUIRE(AlmostEqual(q.y, 0.f));
  REQUIRE(AlmostEqual(q.z, 0.f));
}

TEST_CASE("Quaternion from angle and axis normalizes", "[mathz][Quaternion]")
{
  mathz::Vec3 axis = {1.f, 0.f, 0.f};
  mathz::Quaternion q(static_cast<float>(M_PI), axis);
  REQUIRE(AlmostEqual(q.w, 0.f, 1e-5f));
  REQUIRE(AlmostEqual(q.x, 1.f, 1e-5f));
  REQUIRE(AlmostEqual(q.y, 0.f));
  REQUIRE(AlmostEqual(q.z, 0.f));
}

TEST_CASE("Quaternion dot product is correct", "[mathz][Quaternion]")
{
  mathz::Quaternion a(1.f, 2.f, 3.f, 4.f);
  mathz::Quaternion b(0.5f, -1.f, 2.f, 0.f);
  float dot = a.Dot(b);
  REQUIRE(AlmostEqual(dot, 1.f * 0.5f + 2.f * -1.f + 3.f * 2.f + 4.f * 0.f));
}

TEST_CASE("Quaternion conjugate inverts xyz", "[mathz][Quaternion]")
{
  mathz::Quaternion q(0.7071f, 0.7071f, 0.f, 0.f);
  mathz::Quaternion c = q.Conjugate();
  REQUIRE(AlmostEqual(c.w, 0.7071f));
  REQUIRE(AlmostEqual(c.x, -0.7071f));
  REQUIRE(AlmostEqual(c.y, 0.f));
  REQUIRE(AlmostEqual(c.z, 0.f));
}

TEST_CASE("Quaternion additon is correct", "[mathz][Quaternion]")
{
  mathz::Quaternion a(1.f, 2.f, 3.f, 4.f);
  mathz::Quaternion b(5.f, 6.f, 7.f, 8.f);
  mathz::Quaternion sum = a + b;
  REQUIRE(AlmostEqual(sum.w, 6.f));
  REQUIRE(AlmostEqual(sum.x, 8.f));
  REQUIRE(AlmostEqual(sum.y, 10.f));
  REQUIRE(AlmostEqual(sum.z, 12.f));
}

TEST_CASE("Quaternion multiplied by scalar is correct", "[mathz][Quaternion]")
{
  mathz::Quaternion q(1.f, -2.f, 3.f, -4.f);
  mathz::Quaternion r = q * 2.f;
  REQUIRE(AlmostEqual(r.w, 2.f));
  REQUIRE(AlmostEqual(r.x, -4.f));
  REQUIRE(AlmostEqual(r.y, 6.f));
  REQUIRE(AlmostEqual(r.z, -8.f));
}

TEST_CASE("Quaternion multiplication is correct", "[mathz][Quaternion]")
{
  mathz::Quaternion a(1.f, 0.f, 1.f, 0.f);
  mathz::Quaternion b(1.f, 0.5f, 0.5f, 0.75f);
  mathz::Quaternion result = a * b;

  REQUIRE(AlmostEqual(result.w, 0.5f));
  REQUIRE(AlmostEqual(result.x, 1.25f));
  REQUIRE(AlmostEqual(result.y, 1.5f));
  REQUIRE(AlmostEqual(result.z, 0.25f));
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
  REQUIRE(AlmostEqual(mag, 1.f));
}
