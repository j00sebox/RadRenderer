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
				{ 4,	 4,		 4,		 4,		 { {67.0f, 5.0f, 5.0f, 7.0f}, {3.0f, 0.0f, 2.0f, 8.5f}, {17.6f, 2.3f, 1.1f, 99.0f}, {17.6f, 2.3f, 1.1f, 99.0f} },		 { {4.0f, 6.0f, 4.0f, 6.0f}, {7.0f, 8.8f, 3.3f, 5.0f}, {19.0f, 2.0f, 66.0f, 4.4f}, {1.0f, 2.0f, 3.0f, 4.0f } },		 { {405.0f, 470.0f, 635.5f, 477.0f}, {58.5f, 39.0f, 169.5f, 60.8f}, {206.4f, 326.04f, 447.59f, 517.94}, {206.4f, 326.04f, 447.59f, 517.94} } }
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

//TEST_F(MatrixTest, add) {
//
//}