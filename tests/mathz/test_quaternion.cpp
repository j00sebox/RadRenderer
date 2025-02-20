#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../../mathz/quaternion.hpp"

TEST_CASE("Add Quaternions", "[mathz][quaternion]")
{
  mathz::Quaternion q1(1.f, 2.f, 3.f, 4.f);
  mathz::Quaternion q2(5.f, 6.f, 7.f, 8.f);

  mathz::Quaternion result = q1 + q2;
  mathz::Quaternion expected(0.56f, 0.82f, 1.08f, 1.34f);

  REQUIRE(result.w == Catch::Approx(expected.w).epsilon(0.01f));
  REQUIRE(result.x == Catch::Approx(expected.x).epsilon(0.01f));
  REQUIRE(result.y == Catch::Approx(expected.y).epsilon(0.01f));
  REQUIRE(result.z == Catch::Approx(expected.z).epsilon(0.01f));
}

TEST_CASE("Multiply Quaternions", "[mathz][quaternion]")
{
  mathz::Quaternion q1(1.f, 2.f, 3.f, 4.f);
  mathz::Quaternion q2(5.f, 6.f, 7.f, 8.f);

  mathz::Quaternion result = q1 * q2;
  mathz::Quaternion expected(-0.83019f, 0.16578f, 0.41504f, 0.33241f);

  REQUIRE(result.w == Catch::Approx(expected.w).epsilon(0.01f));
  REQUIRE(result.x == Catch::Approx(expected.x).epsilon(0.01f));
  REQUIRE(result.y == Catch::Approx(expected.y).epsilon(0.01f));
  REQUIRE(result.z == Catch::Approx(expected.z).epsilon(0.01f));
}

TEST_CASE("Quaternion Conjugate", "[mathz][quaternion]")
{
  mathz::Quaternion q(1.f, 2.f, 3.f, 4.f);

  mathz::Quaternion result = q.Conjugate();

  REQUIRE(result.w == Catch::Approx(0.18257f).epsilon(0.01f));
  REQUIRE(result.x == Catch::Approx(-0.36515f).epsilon(0.01f));
  REQUIRE(result.y == Catch::Approx(-0.54722f).epsilon(0.01f));
  REQUIRE(result.z == Catch::Approx(-0.73030f).epsilon(0.01f));
}
