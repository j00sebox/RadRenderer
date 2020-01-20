#include "Matrix.h"

Matrix::Matrix(int row, int col)
{
	r = row;
	c = col;
	value.resize(row, std::vector<float>(col));
}

// Copies vector to this classes vector
void Matrix::Assign(std::vector< std::vector<float> > v)
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
void Matrix::clip()
{
	for (auto vec : value)
		vec.pop_back();
	c--;
}

Matrix Matrix::transpose()
{
	Matrix res(c, r);
	std::vector< std::vector<float> > result;
	result.resize(c, std::vector<float>(r));

	for (int i = 0; i < c; i++)
		for (int j = 0; j < r; j++)
			result[i][j] = value[j][i];

	res.Assign(result);

	return res;
}


Matrix Matrix::operator + (Matrix const& obj)
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

Matrix Matrix::operator - (Matrix const& obj)
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
Matrix Matrix::operator * (Matrix const& obj)
{
	Matrix res(r, obj.c);
	std::vector< std::vector<float> > result;
	result.resize(r, std::vector<float>(obj.c));

	// Check for proper dimensions
	if (value[0].size() == obj.value.size())
	{

		for (int i = 0; i < r; i++)
		{
			for (int k = 0; k < obj.c; k++)
			{
				for (int j = 0; j < obj.r; j++)
					result[i][k] += value[i][j] * obj.value[j][k];
			}
		}
	}
	else
		throw "Maxtrices are not of same dimension!";

	res.Assign(result);

	return res;
}

Matrix Matrix::operator / (float divisor)
{
	Matrix res(r, c);
	std::vector< std::vector<float> > result;
	result.resize(r, std::vector<float>(c));
	result = value;

	if (divisor != 0.0f)
	{

		for (int i = 0; i < r; i++)
		{

			for (int j = 0; j < c; j++)
				result[i][j] /= divisor;
		}

		res.Assign(result);
	}
	else
	{
		return *this;
	}

	return res;
}

// allows value to be accessed easier
float Matrix::operator () (int i, int j)
{
	return value[i][j];
}