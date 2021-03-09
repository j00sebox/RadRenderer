#include <algorithm>
#include <string>
#include <fstream>
#include <strstream>

/*
 - Library provided from https://github.com/OneLoneCoder
 - Used to draw shapes to the console
*/
#include "olcConsoleGameEngine.h"

#include "Matrix4x4.h"

#define CUBE_DEMO

class Triangle
{
public:

	Triangle();

	Triangle(std::vector< std::vector<float > > v);

	std::vector<Vector3D<float>> vertices;

	wchar_t symbol;
	short colour;
};

class RadRenderer : public olcConsoleGameEngine
{
public:
	RadRenderer() {}

	bool OnUserCreate() override;

	bool OnUserUpdate(float fElapsedTime) override;
	
	inline void point_at(Vector3D<float>& point_to, Vector3D<float>& forward, Vector3D<float>& up, Matrix4x4<float>& pMatrix4x4);

	inline Matrix4x4<float> look_at(Matrix4x4<float>& pointAt);

	Vector3D<float>& line_plane_intersect(Vector3D<float>& point, Vector3D<float>& plane_normal, Vector3D<float>& line_begin, Vector3D<float>& line_end);

	int triangle_clip(Vector3D<float>& point, Vector3D<float>& plane_normal, Triangle& ref_tri, Triangle& res_tri1, Triangle& res_tri2);

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

	// 4x4 Matrix4x4 object transformations
	Matrix4x4<float> projection_Matrix4x4, z_rotation, x_rotation, y_rotation, cam_dir, cam_inv;

	// Projection triangles
	Triangle pro, rX, rZ, viewed;

	// Vectors to calculate the normal of triangle faces.
	Vector3D<float> l1;
	Vector3D<float> l2;
	Vector3D<float> normal;

	// Position of camera aka user's view
	Vector3D<float> cam;

	// Direction that the light would be pointing in game
	Vector3D<float> lighting;

	std::vector<Triangle> renderTriangles;

	Vector3D<float> vUp; // current up vector
	Vector3D<float> look_dir; // current forward vector aka where the camera is looking
	Vector3D<float> target; // where the camera is being told to look aka new forward vector
	Vector3D<float> fVelocity; // velocity of camera in forward direction

	Vector3D<float> camera_plane;
	Triangle clipped_tris[2]; // holds the new triangles after clipping if any
	int num_clipped = 0; // holds the number of triangles produced by clippinf function

};
