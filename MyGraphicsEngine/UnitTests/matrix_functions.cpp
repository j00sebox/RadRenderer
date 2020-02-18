#include "gtest/gtest.h"
#include "Matrix.h"

class MatrixTest : public ::testing::Test
{
protected:
	void SetUp() override {

	}
};

TEST_F(MatrixTest, multiply) {
	Matrix test(3, 2);
	Matrix oTest(3, 3);

	std::vector< std::vector<float> > expected_res = { {231.0f, 184.0f}, {50.0f, 22.0f}, {101.0f, 210.0f} };

	oTest = { {13.0f, 12.0f, 5.0f}, {3.0f, 0.0f, 2.0f}, {17.0f, 2.0f, 1.0f} };
	test = { {4.0f, 6.0f}, {7.0f, 8.0f}, {19.0f, 2.0f} };
	

	Matrix res = oTest * test;

	EXPECT_EQ(res.value, expected_res);
}