#include <vector>
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
	Matrix(int row, int col)
	{
		r = row;
		c = col;
		value.resize(row, std::vector<int>(col));
	}

	// Copies vector to this classes vector
	void Assign(std::vector< std::vector<int> > v)
	{
		// Check for proper dimensions
		if (value.size() == v.size() && value[0].size() == v[0].size())
		{
			value = v;
		}
		else
			throw "Maxtrices are not of same dimension!";
	}


	Matrix operator + (Matrix const& obj)
	{
		Matrix res(r, c);
		std::vector< std::vector<int> > result;
		result.resize(r, std::vector<int>(c));

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
		std::vector< std::vector<int> > result;
		result.resize(r, std::vector<int>(c));

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
		std::vector< std::vector<int> > result;
		result.resize(r, std::vector<int>(obj.c));

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

	// Holds values in proper structure
	std::vector< std::vector<int> > value;

private:
	// Holds the values for rows and columns of this object
	int r;
	int c;

};

int main()
{
	std::vector< std::vector<int> > v = { { 0, 1, 1 }, { 0, 1, 1 } };
	std::vector< std::vector<int> > d = { { 1, 1 }, { 1, 1 }, { 1, 1 } };
	Matrix m(2, 3);
	Matrix n(3, 2);
	n.Assign(d);
	m.Assign(v);
	Matrix result(2, 3);
	result = m * n;
	std::cout << result.value[0][1];
	return 0;
}

