#include "gtest/gtest.h"
#include "EmazingEngine.h"
#include <array>

class TriangleTest : public ::testing::Test
{
protected:
	void SetUp() override {

	}
};

TEST_F(TriangleTest, TriangleInit)
{
	Triangle no_param;

	EXPECT_FLOAT_EQ(no_param.vertices[0].x, 0.0f);
	EXPECT_FLOAT_EQ(no_param.vertices[1].x, 0.0f);
	EXPECT_FLOAT_EQ(no_param.vertices[2].x, 0.0f);

	no_param.vertices[0].x += 1.0f;
	no_param.vertices[0].y += 1.0f;
	no_param.vertices[0].z += 1.0f;

	EXPECT_FLOAT_EQ(no_param.vertices[0].x, 1.0f);
	EXPECT_FLOAT_EQ(no_param.vertices[0].y, 1.0f);
	EXPECT_FLOAT_EQ(no_param.vertices[0].z, 1.0f);

	std::vector< std::vector<float> > test_vec = { {0.0f, 60.0f, 6.0f},    {102.0f, 11.0f, -3.0f},    {1.4f, 1.1f, -5.0f} };

	Triangle with_param(test_vec);

	EXPECT_FLOAT_EQ(with_param.vertices[0].x, 0.0f);
	EXPECT_FLOAT_EQ(with_param.vertices[1].x, 102.0f);
	EXPECT_FLOAT_EQ(with_param.vertices[2].x, 1.4f);

	with_param.vertices[0].x -= 1.0f;
	with_param.vertices[0].y -= 1.0f;
	with_param.vertices[0].z -= 1.0f;

	EXPECT_FLOAT_EQ(with_param.vertices[0].x, -1.0f);
	EXPECT_FLOAT_EQ(with_param.vertices[0].y, 59.0f);
	EXPECT_FLOAT_EQ(with_param.vertices[0].z, 5.0f);
}

TEST_F(TriangleTest, CubeDemo)
{
	std::vector<Triangle> test_cube;

		// south
	test_cube.push_back(Triangle({ {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f} }));
	test_cube.push_back(Triangle({ {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f} }));
	test_cube.push_back(Triangle({ {1.0f, 0.0f, 0.0f},    {1.0f, 1.0f, 0.0f},    {1.0f, 1.0f, 1.0f} }));
	test_cube.push_back(Triangle({ {1.0f, 0.0f, 0.0f},    {1.0f, 1.0f, 1.0f},    {1.0f, 0.0f, 1.0f} }));
	test_cube.push_back(Triangle({ {1.0f, 0.0f, 1.0f},    {1.0f, 1.0f, 1.0f},    {0.0f, 1.0f, 1.0f} }));
	test_cube.push_back(Triangle({ {1.0f, 0.0f, 1.0f},    {0.0f, 1.0f, 1.0f},    {0.0f, 0.0f, 1.0f} }));
	test_cube.push_back(Triangle({ {0.0f, 0.0f, 1.0f},    {0.0f, 1.0f, 1.0f},    {0.0f, 1.0f, 0.0f} }));
	test_cube.push_back(Triangle({ {0.0f, 0.0f, 1.0f},    {0.0f, 1.0f, 0.0f},    {0.0f, 0.0f, 0.0f} }));
	test_cube.push_back(Triangle({ {0.0f, 1.0f, 0.0f},    {0.0f, 1.0f, 1.0f},    {1.0f, 1.0f, 1.0f} }));
	test_cube.push_back(Triangle({ {0.0f, 1.0f, 0.0f},    {1.0f, 1.0f, 1.0f},    {1.0f, 1.0f, 0.0f} }));
	test_cube.push_back(Triangle({ {1.0f, 0.0f, 1.0f},    {0.0f, 0.0f, 1.0f},    {0.0f, 0.0f, 0.0f} }));
	test_cube.push_back(Triangle({ {1.0f, 0.0f, 1.0f},    {0.0f, 0.0f, 0.0f},    {1.0f, 0.0f, 0.0f} }));

		// north                         

		// west                                                      
		

		// top                                                      

	std::vector< std::vector<float> > first_tri = { {0.0f, 0.0f, 0.0f}, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f } };

	EXPECT_FLOAT_EQ(test_cube[0].vertices[0].x, first_tri[0][0]);

	std::vector< std::vector<float> > third_tri = { {1.0f, 0.0f, 0.0f}, { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } };

	EXPECT_FLOAT_EQ(test_cube[2].vertices[1].y, third_tri[1][1]);


	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
			EXPECT_FLOAT_EQ(test_cube[0].vertices[i](0, j), first_tri[i][j]);
	}
	
}