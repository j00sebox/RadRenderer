#include <vector>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <numeric>
#include <algorithm>

/**********************************************
class Matrix
@brief Class used to represent matrices with
overloaded operator functions for proper
matrix operations
************************************************/
class Matrix
{
public:
	Matrix() {};

	Matrix(int row, int col);

	void set(int row, int col)
	{
		r = row;
		c = col;
		value.resize(row, std::vector<float>(col));
	}

	// Reduce the amount of columns by 1
	void clip();

	Matrix transpose();

	Matrix inverse();

	Matrix operator + (Matrix const& obj);

	Matrix operator - (Matrix const& obj);

	// Implements the dot product between two matrices
	Matrix operator * (Matrix& obj);

	Matrix operator / (float divisor);

	Matrix& operator = (std::vector< std::vector<float> > val);

	//void operator = (Matrix& m);

	// allows value to be accessed easier
	float operator () (int i, int j);

	//// Holds values in proper structure
	std::vector< std::vector<float> > value;

	// Holds the values for rows and columns of this object
	int r;
	int c;

	float determinant(std::vector< std::vector<float> > dmatrix);

};

/**********************************************
class Vector3D
@brief Class used to represent a vector in 3D 
		space. Inheirets from the Matrix class.
@overrides Assign()
************************************************/
class Vector3D : public Matrix
{
public:
	// Creates a 1x3 matrix
	Vector3D() : Matrix(1, 3) { }

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

	// For easier assignment
	void operator = (std::vector<float> f)
	{
		x = f[0]; y = f[1]; z = f[2];
	}

	// Assign a matrix value to the vector if it has the correct dim
	void operator = (Matrix& m)
	{
		if (m.c == 3 && m.r == 1)
		{
			value = m.value;
		}
	}

	// vector coordinates
	float& x = value[0][0];
	float& y = value[0][1];
	float& z = value[0][2];
};