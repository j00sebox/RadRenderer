#include <algorithm>
#include <string>
#include <fstream>
#include <strstream>

/*
 - Library provided from https://github.com/OneLoneCoder
 - Used to draw shapes to the console
*/
#include "olcConsoleGameEngine.h"

#include "math/Matrix.h"
#include "math/Vector.h"

struct Triangle
{
	math::Vec3<float> vertices[3];

	wchar_t symbol;
	short colour;
};

class RadRenderer : public olcConsoleGameEngine
{
public:
	RadRenderer() {}

	bool OnUserCreate() override;

	bool OnUserUpdate(float fElapsedTime) override;
	
	inline void point_at(math::Vec3<float>& point_to, math::Vec3<float>& forward, math::Vec3<float>& up, math::Mat4<float>& pMat4);

	inline math::Mat4<float> look_at(math::Mat4<float>& pointAt);

	math::Vec3<float>& line_plane_intersect(math::Vec3<float>& point, math::Vec3<float>& plane_normal, math::Vec3<float>& line_begin, math::Vec3<float>& line_end);

	int triangle_clip(math::Vec3<float>& point, math::Vec3<float>& plane_normal, Triangle& ref_tri, Triangle& res_tri1, Triangle& res_tri2);

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

	// 4x4 Mat4 object transformations
	math::Mat4<float> projection_Mat4, z_rotation, x_rotation, y_rotation, cam_dir, cam_inv;

	// Projection triangles
	Triangle pro, rX, rZ, viewed;

	// Vectors to calculate the normal of triangle faces.
	math::Vec3<float> l1;
	math::Vec3<float> l2;
	math::Vec3<float> normal;

	// Position of camera aka user's view
	math::Vec3<float> cam;

	// Direction that the light would be pointing in game
	math::Vec3<float> lighting;

	std::vector<Triangle> renderTriangles;

	math::Vec3<float> vUp; // current up vector
	math::Vec3<float> look_dir; // current forward vector aka where the camera is looking
	math::Vec3<float> target; // where the camera is being told to look aka new forward vector
	math::Vec3<float> fVelocity; // velocity of camera in forward direction

	math::Vec3<float> camera_plane;
	Triangle clipped_tris[2]; // holds the new triangles after clipping if any
	int num_clipped = 0; // holds the number of triangles produced by clippinf function

};
