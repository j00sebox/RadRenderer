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
		throw std::invalid_argument("Maxtrices are not of same dimension!");
}

// Reduce the amount of columns by 1
void Matrix::clip()
{
	for (auto vec : value)
		vec.pop_back();
	c--;
}

// Inverts the rows and columns of a matrix
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

// calculates the inverse of a matrix
Matrix Matrix::inverse()
{
	Matrix inverse(r, c);
	if (r == c)
	{
		float det = determinant(value);
		float subd = 0;
		Matrix adjugate(r, c);

		std::vector<Matrix> sub_matrices;

		Matrix submatrix(r, c);

		for (int i = 0; i < r; i++)
		{
			for (int j = 0; j < c; j++)
			{
				submatrix.value = value;
				submatrix.value.erase(submatrix.value.begin() + i);
				for (int k = 0; k < submatrix.value.size(); k++)
					submatrix.value[k].erase(submatrix.value[k].begin() + j);

				subd = determinant(submatrix.value);
				adjugate.value[j][i] = pow(-1, i + j + 2) * subd;
			}
		}

		inverse = adjugate / det;
	}
	else
		throw std::invalid_argument("Maxtrix must have same number of rows and columns!");

	return inverse;
}

// Calculates deteminant of a given matrix value
float Matrix::determinant(std::vector< std::vector<float> > dmatrix)
{
	int col, row;
	float result = 0;
	if (dmatrix.size() > 2)
	{
		col = dmatrix[0].size();
		row = dmatrix.size();

		std::vector< std::vector<float> > miniMatrix;
		
		for (int j = 0; j < col; j++)
		{
			miniMatrix = dmatrix;
			miniMatrix.erase(miniMatrix.begin());
			for (int k = 0; k < miniMatrix.size(); k++)
				miniMatrix[k].erase(miniMatrix[k].begin() + j);

			result += pow(-1, j)*dmatrix[0][j]*determinant(miniMatrix);
		}	
	}
	else // matrix should now be in a 2x2 shape
	{
		result = dmatrix[0][0] * dmatrix[1][1] - dmatrix[1][0] * dmatrix[0][1];
	}

	return result;

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
		throw std::invalid_argument("Maxtrices are not of same dimension!");

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
		throw std::invalid_argument("Maxtrices are not of same dimension!");

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
		throw std::invalid_argument("Maxtrices are not of same dimension!");

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

void Matrix::operator = (std::vector< std::vector<float> > val)
{
	value = val;
}