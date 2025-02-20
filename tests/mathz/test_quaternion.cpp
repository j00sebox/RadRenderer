#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include "../../mathz/quaternion.hpp"

TEST_CASE("Add quaternions", "[example]")
{
  mathz::Quaternion q1;
  mathz::Quaternion q2;

  mathz::Quaternion result_quaternion = q1 * q2;
  mathz::Quaternion expected_quaternion;

  REQUIRE(expected_quaternion == result_quaternion);
}
