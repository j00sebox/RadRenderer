#include <algorithm>
#include <string>
#include <fstream>
#include <strstream>

/*
 - Library provided from https://github.com/OneLoneCoder
 - Used to draw shapes to the console
*/
#include "olcConsoleGameEngine.h"

#include "Matrix.h"

#define CUBE_DEMO

class Triangle
{
public:

	Triangle();

	Triangle(std::vector< std::vector<float > > v);
	

	std::vector<Vector3D> vertices;

	wchar_t symbol;
	short colour;
};


class EmazingEngine : public olcConsoleGameEngine
{
public:
	EmazingEngine() : projection_matrix(4, 4), x_rotation(4, 4), z_rotation(4, 4) {}

	bool OnUserCreate() override;

	bool OnUserUpdate(float fElapsedTime) override;

	// Map 3D coordinates to 2D space
	inline void coordinate_projection(Vector3D& vertex, Matrix& operation, Vector3D& outVec);
	

	/*inline void coordinate_projection(Vector3D& vertex, Matrix& operation, Vector3D& outVec)
	{
		Matrix tempVec(1, 4);

		tempVec = { { vertex.x, vertex.y, vertex.z, 1.0f } };

		Matrix res = tempVec * operation;

		res = res / res(0, 3);

		res.clip();

		outVec = res;

	}*/

	/*inline Matrix point_at(Vector3D& point_to, Vector3D& forward, Vector3D& up)
	{
		Vector3D nForward;
		forward.subtract(point_to, nForward);
		nForward.normalize();

		Vector3D temp = nForward.multiply(up.dot(nForward));
		Vector3D nUp;
		up.subtract(temp, nUp);
		nUp.normalize();

		Vector3D nRight;
		nUp.cross(nForward, nRight);

		Matrix matrix;
		matrix.m[0][0] = nRight.x;		matrix.m[0][1] = nRight.y;		matrix.m[0][2] = nRight.z;		matrix.m[0][3] = 0.0f;
		matrix.m[1][0] = nUp.x;			matrix.m[1][1] = nUp.y;			matrix.m[1][2] = nUp.z;			matrix.m[1][3] = 0.0f;
		matrix.m[2][0] = nForward.x;	matrix.m[2][1] = nForward.y;	matrix.m[2][2] = nForward.z;	matrix.m[2][3] = 0.0f;
		matrix.m[3][0] = point_to.x;	matrix.m[3][1] = point_to.y;	matrix.m[3][2] = point_to.z;	matrix.m[3][3] = 1.0f;

		return matrix;
	}

	inline Matrix look_at(Matrix& pointAt)
	{
		Matrix matrix;
		matrix.m[0][0] = pointAt.m[0][0]; matrix.m[0][1] = pointAt.m[1][0]; matrix.m[0][2] = pointAt.m[2][0]; matrix.m[0][3] = 0.0f;
		matrix.m[1][0] = pointAt.m[0][1]; matrix.m[1][1] = pointAt.m[1][1]; matrix.m[1][2] = pointAt.m[2][1]; matrix.m[1][3] = 0.0f;
		matrix.m[2][0] = pointAt.m[0][2]; matrix.m[2][1] = pointAt.m[1][2]; matrix.m[2][2] = pointAt.m[2][2]; matrix.m[2][3] = 0.0f;
		matrix.m[3][0] = -(pointAt.m[3][0] * matrix.m[0][0] + pointAt.m[3][1] * matrix.m[1][0] + pointAt.m[3][2] * matrix.m[2][0]);
		matrix.m[3][1] = -(pointAt.m[3][0] * matrix.m[0][1] + pointAt.m[3][1] * matrix.m[1][1] + pointAt.m[3][2] * matrix.m[2][1]);
		matrix.m[3][2] = -(pointAt.m[3][0] * matrix.m[0][2] + pointAt.m[3][1] * matrix.m[1][2] + pointAt.m[3][2] * matrix.m[2][2]);
		matrix.m[3][3] = 1.0f;
		return matrix;
	}*/

	// Loads vertex and face data from txt file realting to obj file
	std::vector<Triangle> LoadOBJFile(std::string fname);
	

private:
	std::vector<Triangle> object;
	float z_far = 1000.0f; // represents the distance from the theoretical distance in the screen to the users face
	float z_near = 0.1f; // represents the distance from the users face to the screen
	float q = z_far / (z_far - z_near);

	float theta = 90.0f; // the field of view for the player
	float thetaRAD = theta / 180.0f * 3.14159f;

	float scaling_factor = 1.0f / tanf(thetaRAD * 0.5f); // amount needed to scale coordinates based on the fov
	float aspect_ratio = (float)ScreenHeight() / (float)ScreenWidth();

	float rotate_angle; // perodically changing to give the appearance that the object is rotating

	Matrix projection_matrix, z_rotation, x_rotation;

	// Projection matrices
	Triangle pro;

	Triangle rX;

	Triangle rZ;

	Triangle viewed;

	// Vectors to calculate the normal of triangle faces.
	Vector3D l1;
	Vector3D l2;
	Vector3D normal;

	// Position of camera aka user's view
	Vector3D cam;

	// Direction that the light would be pointing in game
	Vector3D lighting;

	std::vector<Triangle> renderTriangles;

	Vector3D vUp;
	Vector3D look_dir;
	Vector3D forward;

};
