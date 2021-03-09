#include "Matrix4x4.h"

// Inverts the rows and columns of a Matrix4x4
template<typename T>
Matrix4x4<T> Matrix4x4<T>::transpose()
{
	Matrix4x4 result;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			result[i][j] = mat[j][i];

	return result;
}

// calculates the inverse of a Matrix4x4
//template<typename T>
//Matrix4x4<T> Matrix4x4<T>::inverse()
//{
//	/*Matrix4x4 inverse;
//	if (r == c)
//	{
//		float det = determinant(mat);
//		float subd = 0;
//		Matrix4x4 adjugate;
//
//		std::vector<Matrix4x4> sub_matrices;
//
//		Matrix4x4 subMat;
//
//		for (int i = 0; i < 4; i++)
//		{
//			for (int j = 0; j < 4; j++)
//			{
//				subMat.mat = mat;
//				subMat.mat.erase(subMat.mat.begin() + i);
//				for (int k = 0; k < subMat.mat.size(); k++)
//					subMatrix4x4.mat[k].erase(subMat.mat[k].begin() + j);
//
//				subd = determinant(subMat.mat);
//				adjugate.mat[j][i] = pow(-1, i + j + 2) * subd;
//			}
//		}
//
//		inverse = (adjugate / det);
//	}
//	else
//		throw std::invalid_argument("Maxtrix must have same number of rows and columns!");*/
//
//	Matrix4x4 inv;
//
//	for (int i = 0; i < 4; i++)
//	{
//		int pivot = mat[i][i];
//
//		if (pivot == 0)
//		{
//			int bestOption = i;
//
//			for (int j = 0; j < 4; j++)
//			{
//				if (std::abs(mat[j][i]) > std::abs(mat[bestOption][i]))
//				{
//					bestOption = j;
//				}
//			}
//
//			if (mat[i][bestOption] == 0)
//			{
//				fprintf(stderr, "Matrix has no inverse!\n");
//			}
//			else
//			{
//				T temp;
//
//				for (int j = 0; j < 4; j++)
//				{
//					temp = mat[i][j];
//					mat[i][j] = mat[bestOption][j];
//					mat[bestOption][j] = temp;
//
//					temp = inv.mat[i][j];
//					inv.mat[i][j] = inv.mat[bestOption][j];
//					inv.mat[bestOption][j] = temp;
//				}
//			}
//		}
//
//		for (int j = 0; j < 4; j++)
//		{
//			T q = mat[j][i] / pivot;
//
//			for (int k = 0; k < 4; k++)
//			{
//				mat[j][k] -= q * mat[i][k];
//				inv.mat[j][k] -= q * inv.mat[i][k];
//			}
//		}
//	}
//
//	for (int i = 0; i < 4; i++)
//	{
//
//		if (mat[i][i] != 1)
//		{
//			for (int j = 0; j < 4; j++)
//			{
//				inv.mat[i][j] /= mat[i][i];
//			}
//		}
//	}
//
//
//	return inv;
//}

// Calculates deteminant of a given Matrix4x4 mat
//template<typename T>
//T Matrix4x4<T>::determinant(std::vector< std::vector<T> > &mat)
//{
//	int col, row;
//	float result = 0;
//	if (mat.size() > 2)
//	{
//		col = mat[0].size();
//		row = mat.size();
//
//		std::vector< std::vector<T> > miniMat;
//		
//		for (int j = 0; j < col; j++)
//		{
//			miniMat = mat;
//			miniMat.erase(miniMat.begin());
//			for (int k = 0; k < miniMat.size(); k++)
//				miniMat[k].erase(miniMat[k].begin() + j);
//
//			result += pow(-1, j)* miniMat[0][j]*determinant(miniMat);
//		}	
//	}
//	else // Matrix4x4 should now be in a 2x2 shape
//	{
//		result = mat[0][0] * mat[1][1] - mat[1][0] * mat[0][1];
//	}
//
//	return result;
//
//}

template<typename T>
void Matrix4x4<T>::matmulVec(Vector3D& vec, Vector3D& newVec)
{
	float w;

	newVec.x = vec.x * mat[0][0] + vec.y * mat[1][0] + vec.z * mat[2][0] + mat[3][0];
	newVec.y = vec.x * mat[0][1] + vec.y * mat[1][1] + vec.z * mat[2][1] + mat[3][1];
	newVec.z = vec.x * mat[0][2] + vec.y * mat[1][2] + vec.z * mat[2][2] + mat[3][2];
	
	w = vec.x * mat[0][3] + vec.y * mat[1][3] + vec.z * mat[2][3] + mat[3][3];

	if (w != 0)
	{
		newVec.x /= w;
		newVec.y /= w;
		newVec.z /= w;
	}
}

//template<typename T>
//Matrix4x4<T> Matrix4x4<T>::operator + (Matrix4x4<T> const& obj)
//{
//	Matrix4x4 res(r, c);
//	std::vector< std::vector<float> > result;
//	result.resize(r, std::vector<float>(c));
//
//	// Check for proper dimensions
//	if (mat.size() == obj.mat.size() && mat[0].size() == obj.mat[0].size())
//	{
//		for (int i = 0; i < r; i++)
//			for (int j = 0; j < c; j++)
//				result[i][j] = mat[i][j] + obj.mat[i][j];
//	}
//	else
//		throw std::invalid_argument("Maxtrices are not of same dimension!");
//
//	res = result;
//
//	return res;
//
//}
//
//template<typename T>
//Matrix4x4<T> Matrix4x4<T>::operator - (Matrix4x4 const& obj)
//{
//	Matrix4x4 res(r, c);
//	std::vector< std::vector<float> > result;
//	result.resize(r, std::vector<float>(c));
//
//	// Check for proper dimensions
//	if (mat.size() == obj.mat.size() && mat[0].size() == obj.mat[0].size())
//	{
//		for (int i = 0; i < r; i++)
//			for (int j = 0; j < c; j++)
//				result[i][j] = mat[i][j] - obj.mat[i][j];
//	}
//	else
//		throw std::invalid_argument("Maxtrices are not of same dimension!");
//
//	res = result;
//
//	return res;
//
//}
//
//float myproduct(float x, float* y) { return x * (*y); }
//
//// Implements the product between two matrices
//template<typename T>
//Matrix4x4<T> Matrix4x4<T>::operator * (Matrix4x4& obj)
//{
//	Matrix4x4 res(r, obj.c);
//	std::vector< std::vector<float> > result;
//	result.resize(r, std::vector<float>(obj.c));
//
//	// Check for proper dimensions
//	if (mat[0].size() == obj.mat.size())
//	{
//		std::vector<float*> colPtr;
//		for (int i = 0; i < obj.r; i++) colPtr.push_back(&obj.mat[i][0]);
//
//		for (int i = 0; i < obj.c; i++)
//		{
//			for (int j = 0; j < r; j++)
//				result[j][i] = std::inner_product(mat[j].begin(), mat[j].end(), colPtr.begin(), 0.0f, std::plus<float>(), myproduct);
//
//			std::transform(colPtr.begin(), colPtr.end(), colPtr.begin(), [](float* x) {return ++x;});
//		}
//
//		/*for (int i = 0; i < r; i++)
//		{
//			for (int k = 0; k < obj.c; k++)
//			{
//				for (int j = 0; j < obj.r; j++)
//					result[i][k] += mat[i][j] * obj.mat[j][k];
//			}
//		}*/
//	}
//	else
//		throw std::invalid_argument("Maxtrices are not of same dimension!");
//
//	res = result;
//
//	return res;
//}
//
//template<typename T>
//Matrix4x4<T> Matrix4x4<T>::operator / (float divisor)
//{
//	Matrix4x4 res(r, c);
//	std::vector< std::vector<float> > result;
//	result.resize(r, std::vector<float>(c));
//	result = mat;
//
//	if (divisor != 0.0f)
//	{
//
//		for (int i = 0; i < r; i++)
//		{
//
//			for (int j = 0; j < c; j++)
//				result[i][j] /= divisor;
//		}
//
//		res = result;
//	}
//
//	return res;
//}

// allows mat to be accessed easier
//template<typename T>
//float Matrix4x4<T>::operator () (int i, int j)
//{
//	return mat[i][j];
//}
