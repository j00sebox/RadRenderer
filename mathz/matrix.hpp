#pragma once

#include <vector>
#include <cmath>
#include <stdexcept>

#include "vector.hpp"

struct rows
{
	float row[4];

	inline float& operator[] (int i)
	{
		return row[i];
	}

	inline const float& operator[] (int i) const
	{
		return row[i];
	}
};

namespace mathz {

	/**********************************************
	class Mat4
	@brief Class used to represent 4x4 matrices with
			float values.
	************************************************/
	class Mat4
	{
	public:
		Mat4() = default;

		Mat4(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33)
		{
			mat[0][0] = m00; mat[0][1] = m01; mat[0][2] = m02; mat[0][3] = m03;
			mat[1][0] = m10; mat[1][1] = m11; mat[1][2] = m12; mat[1][3] = m13;
			mat[2][0] = m20; mat[2][1] = m21; mat[2][2] = m22; mat[2][3] = m23;
			mat[3][0] = m30; mat[3][1] = m31; mat[3][2] = m32; mat[3][3] = m33;
		};

		// this constructor just makes setting up the tests easier
		Mat4(std::vector<std::vector<float>> v)
		{
			if (v.size() != 4 || v[0].size() != 4)
				fprintf(stderr, "Passed in vector of wrong size! Must be  4x4.");
			else
			{
				for (int row = 0; row < 4; ++row)
					for (int column = 0; column < 4; column++)
						mat[row][column] = v[row][column];
			}
		}

		void set(
			float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33)
		{
			mat[0][0] = m00; mat[0][1] = m01; mat[0][2] = m02; mat[0][3] = m03;
			mat[1][0] = m10; mat[1][1] = m11; mat[1][2] = m12; mat[1][3] = m13;
			mat[2][0] = m20; mat[2][1] = m21; mat[2][2] = m22; mat[2][3] = m23;
			mat[3][0] = m30; mat[3][1] = m31; mat[3][2] = m32; mat[3][3] = m33;
		};

		// swap rows are columns of the matrix
		constexpr Mat4 transpose()
		{
			Mat4 result;

			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					result.mat[i][j] = mat[j][i];

			return result;
		}

		// computes the inverse of the matrix using the Gauss-Jordan elimination method
		constexpr Mat4 inverse() const
		{
			// an identity matrix is needed to keep track of all the operations done on the original matrix
			// this ends up becoming the inverse
			Mat4 inv;
			Mat4 orig(*this);

			// loop through matrix to set all elements to 0 except for the pivot points
			for (int i = 0; i < 4; ++i)
			{
				// pivot points are the elements on the diagonal
				float pivot = orig.mat[i][i];

				// if a pivot is 0 it must be swapped with another row that is a value other than 0
				if (pivot == 0)
				{
					// current best choice for row to swap with
					int bestOption = i;

					// check all elements in the column for the largest absolute value
					for (int j = 0; j < 4; ++j)
					{
						if (std::abs(orig.mat[j][i]) > std::abs(orig.mat[bestOption][i]))
						{
							bestOption = j;
						}
					}

					// if all the elements in the column are 0 then this matrix is Singular
					if (orig.mat[i][bestOption] == 0)
					{
						fprintf(stderr, "Matrix has no inverse!\n");
					}
					else // otherwise swap the current row the best option row
					{
						float temp = 0.f;

						for (int j = 0; j < 4; ++j)
						{
							temp = orig.mat[i][j];
							orig.mat[i][j] = orig.mat[bestOption][j];
							orig.mat[bestOption][j] = temp;

							temp = inv.mat[i][j];
							inv.mat[i][j] = inv.mat[bestOption][j];
							inv.mat[bestOption][j] = temp;
						}
					}
				}

				// set rest of elements in column to 0
				// this is done by calculating a coefficient q based on the element that we want to set to 0 over the pivot
				// then subtracting the desired row by q * the current row
				for (int j = 0; j < 4; ++j)
				{
					if (i != j)
					{
						float q = orig.mat[j][i] / orig.mat[i][i];

						if (q != 0)
						{
							for (int k = 0; k < 4; ++k)
							{
								orig.mat[j][k] -= q * orig.mat[i][k];
								inv.mat[j][k] -= q * inv.mat[i][k];
							}
						}

					}
				}
			}

			// now set all pivots to one to get the identity matrix
			for (int i = 0; i < 4; ++i)
			{

				if (orig.mat[i][i] != 1)
				{
					for (int j = 0; j < 4; j++)
					{
						inv.mat[i][j] /= orig.mat[i][i];
					}
				}
			}

			return inv;
		}

		// add two matrices together
		constexpr Mat4 operator + (Mat4 const& obj)
		{
			Mat4 res;

			for (int row = 0; row < 4; ++row)
				for (int column = 0; column < 4; ++column)
					res.mat[row][column] = mat[row][column] + obj.mat[row][column];

			return res;
		}

		// subtract matrix from another
		constexpr Mat4 operator - (Mat4 const& obj)
		{
			Mat4 res;

			for (int row = 0; row < 4; ++row)
				for (int column = 0; column < 4; ++column)
					res.mat[row][column] = mat[row][column] - obj.mat[row][column];

			return res;
		};

		constexpr Mat4 operator* (const Mat4& other_mat) const
		{
			Mat4 res_mat;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					res_mat[i][j] =
						mat[i][0] * other_mat.mat[0][j] +
						mat[i][1] * other_mat.mat[1][j] +
						mat[i][2] * other_mat.mat[2][j] +
						mat[i][3] * other_mat.mat[3][j];
				}
			}

			return res_mat;
		}

		inline Vec3 operator* (const Vec3& vec) const
		{
			float w = 0.f;
			 
			Vec3 new_vec;

			new_vec.x = vec.x * mat[0][0] + vec.y * mat[1][0] + vec.z * mat[2][0] + mat[3][0];
			new_vec.y = vec.x * mat[0][1] + vec.y * mat[1][1] + vec.z * mat[2][1] + mat[3][1];
			new_vec.z = vec.x * mat[0][2] + vec.y * mat[1][2] + vec.z * mat[2][2] + mat[3][2];

			w = vec.x * mat[0][3] + vec.y * mat[1][3] + vec.z * mat[2][3] + mat[3][3];

			if (w != 0)
			{
				new_vec.x /= w;
				new_vec.y /= w;
				new_vec.z /= w;
			}

			return new_vec;
		}

		constexpr Mat4 operator* (float scalar) const
		{
			Mat4 res_mat;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					res_mat[i][j] = mat[i][j] * scalar;
				}
			}

			return res_mat;
		}

		constexpr void operator*= (const Mat4& other_mat)
		{
			Mat4 temp = *this * other_mat;
			*this = temp;
		}

		inline rows& operator[] (int i)
		{
			return mat[i];
		}

		inline const rows& operator[] (int i) const
		{
			return mat[i];
		}

		void clear()
		{
			set(1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
			);
		}

		// Default matrix is identity
		rows mat[4] = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };
	};
}
