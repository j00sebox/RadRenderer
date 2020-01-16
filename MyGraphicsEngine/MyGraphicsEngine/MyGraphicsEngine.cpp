#include <vector>
#include <iostream>

/*
 - Library provided from https://github.com/OneLoneCoder
 - Used to draw shapes to the console 
*/
#include "olcConsoleGameEngine.h"

/**********************************************
class Matrix
@brief Class used to represent matrices with
overloaded operator functions for proper
matrix operations
************************************************/
class Matrix
{
public:
	Matrix(int row, int col)
	{
		r = row;
		c = col;
		value.resize(row, std::vector<float>(col));
	}

	// Copies vector to this classes vector
	void Assign(std::vector< std::vector<float> > v)
	{
		// Check for proper dimensions
		if (value.size() == v.size() && value[0].size() == v[0].size())
		{
			value = v;
		}
		else
			throw "Maxtrices are not of same dimension!";
	}

	// Reduce the amount of columns by 1
	void clip()
	{
		for (auto vec : value)
			vec.pop_back();
		c--;
	}


	Matrix operator + (Matrix const& obj)
	{
		Matrix res(r, c);
		std::vector< std::vector<float> > result;
		result.resize(r, std::vector<float>(c));

		// Check for proper dimensions
		if (value.size() == obj.value.size() && value[0].size() == obj.value[0].size())
		{
			for (int i = 0; i < r; i++)
			for (int j = 0; j < c; j++)
				result[i][j] = value[i][j] + obj.value[i][j];
		}
		else
			throw "Maxtrices are not of same dimension!";

		res.Assign(result);

		return res;

	}

	Matrix operator - (Matrix const& obj)
	{
		Matrix res(r, c);
		std::vector< std::vector<float> > result;
		result.resize(r, std::vector<float>(c));

		// Check for proper dimensions
		if (value.size() == obj.value.size() && value[0].size() == obj.value[0].size())
		{
			for (int i = 0; i < r; i++)
			for (int j = 0; j < c; j++)
				result[i][j] = value[i][j] - obj.value[i][j];
		}
		else
			throw "Maxtrices are not of same dimension!";

		res.Assign(result);

		return res;

	}

	// Implements the dot product between two matrices
	Matrix operator * (Matrix const& obj)
	{
		Matrix res(r, obj.c);
		std::vector< std::vector<float> > result;
		result.resize(r, std::vector<float>(obj.c));

		// Check for proper dimensions
		if (value[0].size() == obj.value.size())
		{
			for (int k = 0; k < r; k++)
			{
				for (int i = 0; i < r; i++)
				{

					for (int j = 0; j < c; j++)
						result[i][k] += value[i][j] * obj.value[j][i];
				}
			}

		}
		else
			throw "Maxtrices are not of same dimension!";

		res.Assign(result);

		return res;
	}

	Matrix operator / (float divisor)
	{
		Matrix res(r, c);
		std::vector< std::vector<float> > result;
		result.resize(r, std::vector<float>(c));
		result = value;


		for (int i = 0; i < r; i++)
		{

			for (int j = 0; j < c; j++)
				result[i][j] /= divisor;
		}

		res.Assign(result);

		return res;
	}

	// Holds values in proper structure
	std::vector< std::vector<float> > value;

private:
	// Holds the values for rows and columns of this object
	int r;
	int c;

};

class Triangle
{
public:
	
	Triangle(std::vector< std::vector<float > > v) : vertices(3, 3)
	{
		vertices.Assign(v);
	}

	Matrix vertices;
};

class EmazingEngine : public olcConsoleGameEngine
{
public:
	bool OnUserCreate() override
	{
		cube =
		{
			Triangle({ { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f } }),
			Triangle({ { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } }),

			Triangle({ { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } }),
			Triangle({ { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f } }),

			Triangle({ { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 1.0f } }),
			Triangle({ { 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } }),

			Triangle({ { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } }),
			Triangle({ { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }),

			Triangle({ { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } }),
			Triangle({ { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f } }),

			Triangle({ { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } }),
			Triangle({ { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } }),
		};

	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear screen to redraw
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

		Matrix pro1(1, 3); 
		Matrix pro2(1, 3);
		Matrix pro3(1, 3);

		for (auto t : cube)
		{
			pro1 = coordinate_projection(t.vertices.value[0][0], t.vertices.value[0][0], t.vertices.value[0][0]);


		}
	}

	// Map 3D coordinates to 2D space
	Matrix coordinate_projection(float x, float y, float z)
	{
		// Put coordinates in vector format
		std::vector< std::vector<float> > coordinates = { { x, y, z, 1 } }; // 1 is added so that -z*near can be subtracted from z*q
		std::vector< std::vector<float> > projection_functions = { { aspect_ratio*scaling_factor, 0, 0, 0 }, { 0, scaling_factor, 0, 0 }, { 0, 0, q, -z*z_near }, { 0, 0, 1, 0 } };
		Matrix coordinate_vec(1, 4);
		Matrix projection_matrix(4, 4);
		coordinate_vec.Assign(coordinates);
		projection_matrix.Assign(projection_functions);

		Matrix res = coordinate_vec * projection_matrix;

		// Dived entire matrix by the last value to convert it back to 3D space
		res = res / res.value[0][3];

		// Change matrix to 1X3
		res.clip();

		return res;
	}

private:
	std::vector<Triangle> cube;
	float z_far = 1000.0f; // represents the distance from the theoretical distance in the screen to the users face
	float z_near = 0.1f; // represents the distance from the users face to the screen
	float q = z_far / (z_far - z_near);

	float theta = 60.0f; // the field of view for the player

	float scaling_factor = 1 / tan(theta / 2); // amount needed to scale coordinates based on the fov
	float aspect_ratio = (float)ScreenHeight() / (float)ScreenWidth();
};









int main()
{
	float x, y, z;


	/*std::vector< std::vector<float> > test = { { 2, 2 } };
	float w = 2;
	Matrix t(1, 2);
	t.Assign(test);

	Matrix res(1, 2);
	res = t / 2;

	std::cout << res.value[0][0] << res.value[0][1];

	while (1);*/

	return 0;
}
