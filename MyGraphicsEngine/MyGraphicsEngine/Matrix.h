#include <vector>
#include <cmath>

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

	void transpose();

	Matrix operator + (Matrix const& obj);

	Matrix operator - (Matrix const& obj);

	// Implements the dot product between two matrices
	Matrix operator * (Matrix const& obj);

	Matrix operator / (float divisor);

	// allows value to be accessed easier
	float operator () (int i, int j);

	// Holds values in proper structure
	std::vector< std::vector<float> > value;

private:
	// Holds the values for rows and columns of this object
	int r;
	int c;

};

class Vector3D : public Matrix
{
public:
	Vector3D() : Matrix(1, 3) {}

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
			throw "Maxtrices are not of same dimension!";
	}

	Vector3D cross(Vector3D line)
	{
		Vector3D normal;

		// Calculate normal vector of the traingle
		normal.x = y * line.z - z * line.y;
		normal.y = z * line.x - x * line.z;
		normal.z = x * line.y - y * line.x;

		return normal;
	}

	void normalize()
	{
		float normalize = sqrtf(exp2(x) + exp2(y) + exp2(z));

		x /= normalize;
		value[0][0] /= normalize;
		y /= normalize;
		value[0][1] /= normalize;
		z /= normalize;
		value[0][2] /= normalize;
	}
	
	float x, y, z;
};