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

//#define CUBE_DEMO

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
	EmazingEngine() : projection_matrix(4, 4), x_rotation(4, 4), z_rotation(4, 4), cam_dir(4, 4), cam_inv(4, 4) {}

	bool OnUserCreate() override;

	bool OnUserUpdate(float fElapsedTime) override;

	// Map 3D coordinates to 2D space
	inline void coordinate_projection(Vector3D& vertex, Matrix& operation, Vector3D& outVec);
	

	inline void point_at(Vector3D& point_to, Vector3D& forward, Vector3D& up, Matrix& pMatrix);

	inline Matrix look_at(Matrix& pointAt);

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

	float facing_dir;

	Matrix projection_matrix, z_rotation, x_rotation, cam_dir, cam_inv;

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
	Vector3D fVelocity;

};
