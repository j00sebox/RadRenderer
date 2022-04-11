#include <algorithm>
#include <string>
#include <fstream>
#include <strstream>

#include "RendererSettings.h"

#include "math/Matrix.h"
#include "math/Vector.h"

struct Pixel
{
	std::uint8_t r, g, b, a;
};

struct Triangle
{
	math::Vec3<float> vertices[3];

	Pixel colour;
};

class RadRenderer
{
public:
	RadRenderer(unsigned int screen_width, unsigned int screen_height, RendererSettings rs = { 0.1f, 1000.f, 80.f });

	~RadRenderer() {}

	Pixel* update(float elapsed_time);

protected:
	void rasterize(int x1, int y1, int x2, int y2, int x3, int y3, const Pixel& col);

	bool edge_function(int x1, int y1, int x2, int y2, const math::Vec2<float>& p);

	Pixel get_colour(float lum);

	void set_pixel(int x, int y, const Pixel& col);

	void clear_frame_buffer();
	
	inline void point_at(math::Vec3<float>& point_to, math::Vec3<float>& forward, math::Vec3<float>& up, math::Mat4<float>& pMat4);

	inline math::Mat4<float> look_at(math::Mat4<float>& pointAt);

	math::Vec3<float>& line_plane_intersect(math::Vec3<float>& point, math::Vec3<float>& plane_normal, math::Vec3<float>& line_begin, math::Vec3<float>& line_end);

	int triangle_clip(math::Vec3<float>& point, math::Vec3<float>& plane_normal, Triangle& ref_tri, Triangle& res_tri1, Triangle& res_tri2);

	// loads vertex and face data from txt file realting to obj file
	std::vector<Triangle> load_obj_file(std::string fname);
	
	inline void transform_tri(Triangle& tri, const math::Mat4<float>& transform);

private:
	std::vector<Triangle> object;

	float m_far, m_near, m_fov; 

	float rotate_angle; // perodically changing to give the appearance that the object is rotating

	float facing_dir;

	// transforms
	math::Mat4<float> m_projection, z_rotation, x_rotation, y_rotation, cam_dir, cam_inv;

	// Vectors to calculate the normal of triangle faces.
	math::Vec3<float> l1;
	math::Vec3<float> l2;
	math::Vec3<float> normal;

	math::Vec3<float> m_camera;

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

	unsigned int m_screen_width, m_screen_height, m_buffer_size;
	std::unique_ptr<Pixel> m_frame_buffer;

};
