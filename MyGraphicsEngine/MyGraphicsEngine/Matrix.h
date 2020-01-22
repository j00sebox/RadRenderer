#include <vector>
#include <cmath>
#include <stdexcept>
#include <iostream>

/**********************************************
class Matrix
@brief Class used to represent matrices with
overloaded operator functions for proper
matrix operations
************************************************/
class Matrix
{
public:
	Matrix(int row, int col);

	// Copies vector to this classes vector
	virtual void Assign(std::vector< std::vector<float> > v);

	// Reduce the amount of columns by 1
	void clip();

	Matrix transpose();

	Matrix inverse();

	Matrix operator + (Matrix const& obj);

	Matrix operator - (Matrix const& obj);

	// Implements the dot product between two matrices
	Matrix operator * (Matrix const& obj);

	Matrix operator / (float divisor);

	void operator = (std::vector< std::vector<float> > val);

	// allows value to be accessed easier
	float operator () (int i, int j);

	// Holds values in proper structure
	std::vector< std::vector<float> > value;

private:
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
	Vector3D() : Matrix(1, 3) { x = 0; y = 0; z = 0; }

	// overridden to assign vector coordinates as well as matrix value
	void Assign(std::vector< std::vector<float> > v) override
	{
		// Check for proper dimensions
		if (value.size() == v.size() && value[0].size() == v[0].size())
		{
			x = v[0][0];
			y = v[0][1];
			z = v[0][2];

			value = v;
		}
		else
			throw std::invalid_argument("Maxtrices are not of same dimension!");
	}

	// computes cross product of two vectors
	Vector3D cross(Vector3D line)
	{
		Vector3D normal;

		// Calculate normal vector of the traingle
		normal.x = y * line.z - z * line.y;
		normal.y = z * line.x - x * line.z;
		normal.z = x * line.y - y * line.x;

		normal.value[0][0] = normal.x;
		normal.value[0][1] = normal.y;
		normal.value[0][2] = normal.z;

		return normal;
	}

	float dot(Vector3D vec)
	{
		return x * vec.x + y * vec.y + z * vec.z;
	}

	/*Vector3D operator / (float div) 
	{
		Vector3D out;

		if (div != 0.0f)
		{
			out.x /= div;
			out.value[0][0] /= div;
			out.y /= div;
			out.value[0][1] /= div;
			out.z /= div;
			out.value[0][2] /= div;
		}

		return out;
	}*/

	// Used to change to unit vector
	void normalize()
	{
		float normalize = sqrtf(pow(x, 2) + pow(y, 2) + pow(z, 2));

		x /= normalize;
		value[0][0] /= normalize;
		y /= normalize;
		value[0][1] /= normalize;
		z /= normalize;
		value[0][2] /= normalize;
	}

	// vector coordinates
	float x, y, z;
};