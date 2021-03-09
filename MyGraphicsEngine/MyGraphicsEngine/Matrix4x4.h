#include <vector>
#include <cmath>
#include <stdexcept>
#include <numeric>
#include <algorithm>
#include <array>


/**********************************************
class Vector3D
@brief Class used to represent a vector in 3D
		space.
************************************************/
class Vector3D
{
public:
	// Creates a 3D vector
	Vector3D() : x(0.0f), y(0.0f), z(0.0f) {};

	// computes cross product of two vectors
	void cross(Vector3D& line, Vector3D& normal)
	{
		// Calculate normal vector of the traingle
		normal.x = y * line.z - z * line.y;
		normal.y = z * line.x - x * line.z;
		normal.z = x * line.y - y * line.x;
	}

	// Calculates the dot product between two vectors
	float dot(Vector3D& vec)
	{
		return x * vec.x + y * vec.y + z * vec.z;
	}

	// Used to change to unit vector
	void normalize()
	{
		float normalize = sqrtf(pow(x, 2) + pow(y, 2) + pow(z, 2));

		x /= normalize;
		y /= normalize;
		z /= normalize;
	}

	void subtract(Vector3D& inputVec, Vector3D& outVec)
	{
		outVec.x = x - inputVec.x;
		outVec.y = y - inputVec.y;
		outVec.z = z - inputVec.z;
	}

	void scalar_mul(Vector3D& outVec, float scalar)
	{
		outVec.x = scalar * x;
		outVec.y = scalar * y;
		outVec.z = scalar * z;
	}

	void add(Vector3D& a, Vector3D& outVec)
	{
		outVec.x = x + a.x;
		outVec.y = y + a.y;
		outVec.z = z + a.z;
	}

	// For easier assignment
	void operator = (std::vector<float> f)
	{
		x = f[0]; y = f[1]; z = f[2];
	}


	Vector3D operator = (Vector3D& const vec)
	{
		x = vec.x; y = vec.y; z = vec.z;

		return *this;
	}

	float x, y, z;

};

/**********************************************
class Matrix4x4
@brief Class used to represent matrices with
overloaded operator functions for proper
Matrix4x4 operations
************************************************/
template<typename T>
class Matrix4x4
{
public:

	T mat[4][4] = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };

	Matrix4x4() {};

	// work around since you can't assign arrays after intialize
	// seems beter than using std::array
	Matrix4x4(T m00, T m01, T m02, T m03,
		      T m10, T m11, T m12, T m13,
			  T m20, T m21, T m22, T m23,
			  T m30, T m31, T m32, T m33 )
	{
		mat[0][0] = m00; mat[0][1] = m01; mat[0][2] = m02; mat[0][3] = m03;
		mat[1][0] = m10; mat[1][1] = m11; mat[1][2] = m12; mat[1][3] = m13;
		mat[2][0] = m20; mat[2][1] = m21; mat[2][2] = m22; mat[2][3] = m23;
		mat[3][0] = m30; mat[3][1] = m31; mat[3][2] = m32; mat[3][3] = m33;
	};


	void set(
		T m00, T m01, T m02, T m03,
		T m10, T m11, T m12, T m13,
		T m20, T m21, T m22, T m23,
		T m30, T m31, T m32, T m33)
	{
		mat[0][0] = m00; mat[0][1] = m01; mat[0][2] = m02; mat[0][3] = m03;
		mat[1][0] = m10; mat[1][1] = m11; mat[1][2] = m12; mat[1][3] = m13;
		mat[2][0] = m20; mat[2][1] = m21; mat[2][2] = m22; mat[2][3] = m23;
		mat[3][0] = m30; mat[3][1] = m31; mat[3][2] = m32; mat[3][3] = m33;
	};

	Matrix4x4 transpose();

	Matrix4x4<T> inverse()
	{
		Matrix4x4 inv;

		for (int i = 0; i < 4; i++)
		{
			int pivot = mat[i][i];

			if (pivot == 0)
			{
				int bestOption = i;

				for (int j = 0; j < 4; j++)
				{
					if (std::abs(mat[j][i]) > std::abs(mat[bestOption][i]))
					{
						bestOption = j;
					}
				}

				if (mat[i][bestOption] == 0)
				{
					fprintf(stderr, "Matrix has no inverse!\n");
				}
				else
				{
					T temp;

					for (int j = 0; j < 4; j++)
					{
						temp = mat[i][j];
						mat[i][j] = mat[bestOption][j];
						mat[bestOption][j] = temp;

						temp = inv.mat[i][j];
						inv.mat[i][j] = inv.mat[bestOption][j];
						inv.mat[bestOption][j] = temp;
					}
				}
			}

			for (int j = 0; j < 4; j++)
			{
				T q = mat[j][i] / pivot;

				for (int k = 0; k < 4; k++)
				{
					mat[j][k] -= q * mat[i][k];
					inv.mat[j][k] -= q * inv.mat[i][k];
				}
			}
		}

		for (int i = 0; i < 4; i++)
		{

			if (mat[i][i] != 1)
			{
				for (int j = 0; j < 4; j++)
				{
					inv.mat[i][j] /= mat[i][i];
				}
			}
		}


		return inv;
	}

	Matrix4x4 operator + (Matrix4x4 const& obj);

	Matrix4x4 operator - (Matrix4x4 const& obj);

	// Implements the dot product between two matrices
	Matrix4x4 operator * (Matrix4x4& obj);

	Matrix4x4 operator / (float divisor);

	//Matrix4x4& operator = (std::vector< std::vector<float> > val);

	void matmulVec(Vector3D& vec, Vector3D& newVec);

	// allows value to be accessed easier
	float operator () (int i, int j) { return mat[i][j]; };
	

	// T determinant(std::vector< std::vector<T> >& mat);

};

