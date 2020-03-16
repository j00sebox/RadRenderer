#include "gtest/gtest.h"
#include "Matrix.h"
#include <array>

class MatrixTest : public ::testing::Test
{
protected:
	void SetUp() override {

	}
};

TEST_F(MatrixTest, multiply) {
	

	// Define all the params needed for the tests
	struct MultiplyTest
	{
		int row1;
		int col1;
		int row2;
		int col2;
		std::vector< std::vector<float> > test_param_1;
		std::vector< std::vector<float> > test_param_2;
		std::vector< std::vector<float> > expected_res;
		
	};

	const int TEST_CASES = 2;

	// Initialize array with each element holding the testing params for a case
	std::array< MultiplyTest, TEST_CASES > test_arr =
	{
		{
			//	row1	col1	row2	col2						test_param_1																	test_param_2									exprected_res	
				
				// Test 1: Different acceptable dimensions. The purpose is to show that if the first matrix rows and second matrix columns are the same dimension then it will work 
				{ 3,	 3,		 3,		 2,		 { {13.0f, 12.0f, 5.0f}, {3.0f, 0.0f, 2.0f}, {17.0f, 2.0f, 1.0f} },			{ {4.0f, 6.0f}, {7.0f, 8.0f}, {19.0f, 2.0f} },		 { {231.0f, 184.0f}, {50.0f, 22.0f}, {101.0f, 120.0f} } },

				// Test 2: Same dimensions. Shows that multiplication works with two matrices with the same dimesnions in this case two 4x4 ones
				{ 4,	 4,		 4,		 4,		 { {67.0f, 5.0f, 5.0f, 7.0f}, {3.0f, 0.0f, 2.0f, 8.5f}, {17.6f, 2.3f, 1.1f, 99.0f}, {17.6f, 2.3f, 1.1f, 99.0f} },		 { {4.0f, 6.0f, 4.0f, 6.0f}, {7.0f, 8.8f, 3.3f, 5.0f}, {19.0f, 2.0f, 66.0f, 4.4f}, {1.0f, 2.0f, 3.0f, 4.0f } },		 { {405.0f, 470.0f, 635.5f, 477.0f}, {58.5f, 39.0f, 169.5f, 60.8f}, {206.4f, 326.04f, 447.59f, 517.94f}, {206.4f, 326.04f, 447.59f, 517.94f} } }
		}
	};

	// loop through test cases
	for (int i = 0; i < TEST_CASES; i++)
	{
		Matrix test_p1(test_arr[i].row1, test_arr[i].col1);
		Matrix test_p2(test_arr[i].row2, test_arr[i].col2);

		test_p1 = test_arr[i].test_param_1;
		test_p2 = test_arr[i].test_param_2;

		Matrix res = test_p1 * test_p2;

		// Evalute if this is true
		EXPECT_EQ(res.value, test_arr[i].expected_res);
	}
}

TEST_F(MatrixTest, addandsubtract) {

	// Define all the params needed for the tests
	struct MultiplyTest
	{
		int row1;
		int col1;
		int row2;
		int col2;
		std::vector< std::vector<float> > test_param_1;
		std::vector< std::vector<float> > test_param_2;
		std::vector< std::vector<float> > expected_res;
		bool add;

	};

	const int TEST_CASES = 2;

	// Initialize array with each element holding the testing params for a case
	std::array< MultiplyTest, TEST_CASES > test_arr =
	{
		{
			//	row1	col1	row2	col2												test_param_1																												test_param_2																													expected_res													add

				// Test 1: Same dimensions addition
				{ 4,	 4,		 4,		 4,		 { {67.0f, 5.0f, 5.0f, 7.0f}, {3.0f, 0.0f, 2.0f, 8.5f}, {17.6f, 2.3f, 1.1f, 99.0f}, {17.6f, 2.3f, 1.1f, 99.0f} },		 { {4.0f, 6.0f, 4.0f, 6.0f}, {7.0f, 8.8f, 3.3f, 5.0f}, {19.0f, 2.0f, 66.0f, 4.4f}, {1.0f, 2.0f, 3.0f, 4.0f } },		 { {71.0f, 11.0f, 9.0f, 13.0f}, {10.0f, 8.8f, 5.3f, 13.5f}, {36.6f, 4.3f, 67.1f, 103.4f}, {18.6f, 4.3f, 4.1f, 103.0f} },	true },

				// Test 2: Same dimensions subtraction
				{ 4,	 4,		 4,		 4,		 { {67.0f, 5.0f, 5.0f, 7.0f}, {11.0f, 90.0f, 4.6f, 8.5f}, {21.4f, 2.3f, 1.1f, 99.0f}, {17.6f, 2.3f, 3.1f, 99.0f} },		 { {4.0f, 4.0f, 4.0f, 6.0f}, {7.0f, 8.8f, 3.3f, 5.0f}, {19.0f, 2.0f, 1.1f, 4.4f}, {1.0f, 2.0f, 3.0f, 4.0f } },		 { {63.0f, 1.0f, 1.0f, 1.0f}, {4.0f, 81.2f, 1.3f, 3.5f}, {2.4f, 0.3f, 0.0f, 94.6f}, {16.6f, 0.3f, 0.1f, 95.0f} },	false }
		}
	};

	// loop through test cases
	for (int i = 0; i < TEST_CASES; i++)
	{
		Matrix test_p1(test_arr[i].row1, test_arr[i].col1);
		Matrix test_p2(test_arr[i].row2, test_arr[i].col2);
		Matrix res(test_arr[i].row1, test_arr[i].col1);

		test_p1 = test_arr[i].test_param_1;
		test_p2 = test_arr[i].test_param_2;

		if (test_arr[i].add) {
			 res = test_p1 + test_p2;
		} 
		else {
			 res = test_p1 - test_p2;
		}

		for (int k = 0; k < test_arr[i].row1; k++)
		{
			for(int j = 0; j < test_arr[i].col1; j++)
				// Evalute if this is true
				EXPECT_NEAR(res(k, j), test_arr[i].expected_res[k][j], 0.0001);
		}
	}
}

TEST_F(MatrixTest, InverseTest)
{
	Matrix inv_test(5, 5);
	inv_test = { { -22.0f, 65.0f, 7.9f, 0.0f, 4.3f },
				 {	5.0f, 33.0f, 0.8f, -1.0f, 4.0f },
				 {  88.0f, 9.0f, 2.5f,  6.6f, 4.0f },
				 {  33.0f, 1.3f, -0.5f, -7.0f, 5.0f },
				 {  2.0f, 9.0f, 94.0f, 4.0f, 2.0f },
	};

	Matrix inv(5, 5);
	inv = { { 0.069200467825893723742f, -0.1425711116236314086f,	0.022392562345822948982f,	0.038627434942696736742f, -0.0049924946267964286688f },
			{ 0.10653193489728533906f, -0.18494622343786515216f,	0.021472462203300888535f,	0.042251760219971247615f, -0.0077255381099630707934f },
			{ 0.02938171666019830568f, -0.06204235128144407175f,	0.004055105371119713144f,	0.01764830886680727167f,   0.0086830288342041808043f },
			{ -0.42589754083377211216f,	0.85528485518830239268f,   -0.046070692230919335927f,  -0.29237328684711731168f,   0.028184603995637206984f  },
			{ -1.0777397762254538913f,	2.1802499169452911789f,    -0.21746721024146479328f,   -0.47348429897827449607f,	0.075285852922759335476f }
	};

	Matrix res = inv_test.inverse();


	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
			EXPECT_NEAR(res(i, j), inv(i, j), 0.00001);
	}

}

class VectorTest : public ::testing::Test
{
protected:
	void SetUp() override {

	}
};

TEST_F(VectorTest, CoordinateTests)
{
	Vector3D test_vec;

	// Give vector initial value
	test_vec = {2.0f, 7.7f, 1.23f};

	// Check to ensure that the coordinates are the same as it's matrix value
	EXPECT_FLOAT_EQ(test_vec(0, 0), test_vec.x);
	EXPECT_FLOAT_EQ(test_vec(0, 1), test_vec.y);
	EXPECT_FLOAT_EQ(test_vec(0, 2), test_vec.z);

	// Increment all the coordinates
	test_vec.x += 4.0f;
	test_vec.y += 4.0f;
	test_vec.z += 4.0f;

	// Check again to ensure they are equal
	EXPECT_FLOAT_EQ(test_vec(0, 0), test_vec.x);
	EXPECT_FLOAT_EQ(test_vec(0, 1), test_vec.y);
	EXPECT_FLOAT_EQ(test_vec(0, 2), test_vec.z);
}

TEST_F(VectorTest, DotProd) 
{
	Vector3D vec1, vec2;
	float res, exp_res;

	exp_res = 156.69;

	vec1 = {12.0f, 7.9f, 1.23f};
	vec2 = {10.3f, 4.5f, -2.0f};

	res = vec1.dot(vec2);

	EXPECT_FLOAT_EQ(res, exp_res);
}

TEST_F(VectorTest, CrossProd)
{
	Vector3D vec1, vec2, res, exp_res;

	exp_res = {-21.335, 36.669, -27.37};

	vec1 = {12.0f, 7.9f, 1.23f};
	vec2= {10.3f, 4.5f, -2.0f};

	vec1.cross(vec2, res);

	EXPECT_FLOAT_EQ(res.x, exp_res.x);
	EXPECT_FLOAT_EQ(res.y, exp_res.y);
	EXPECT_FLOAT_EQ(res.z, exp_res.z);
}