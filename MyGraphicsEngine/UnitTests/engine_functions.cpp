#include "gtest/gtest.h"
#include "EmazingEngine.h"
#include <array>

class EngineTest : public ::testing::Test
{
protected:
	void SetUp() override {

	}
};


TEST_F(EngineTest, CoordinateProjection)
{
	Matrix projection_matrix(4, 4);
	projection_matrix = { { 73.03975f, 0.0f, 0.0f, 0.0f }, { 0.0f, 73.03975f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } , { 0.0f, 0.0f, -0.1f, 0.0f } };

	Vector3D input;
	input = { 2.0f, 26.0f, -1.23f };
	Vector3D output;

	EmazingEngine test;

	test.coordinate_projection(input, projection_matrix, output);


	EXPECT_NEAR(output.x, -118.7638f, 0.01);
	EXPECT_NEAR(output.y, -1543.9297f, 0.01);
	EXPECT_NEAR(output.z, 1.08f, 0.01);

}