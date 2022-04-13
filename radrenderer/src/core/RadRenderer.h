#include "RendererSettings.h"
#include "Object.h"
#include "Camera.h"

#include <memory>

class RadRenderer
{
public:
	RadRenderer(unsigned int screen_width, unsigned int screen_height, RendererSettings rs = { 0.1f, 1000.f, 80.f });

	~RadRenderer() {}

	Pixel* update(float elapsed_time, float cam_forward, float rotate_x, float rotate_y);

protected:
	void rasterize(const Triangle& t);

	bool edge_function(int x1, int y1, int x2, int y2, const math::Vec2<float>& p);

	Pixel get_colour(float lum);

	void set_pixel(int x, int y, const Pixel& col);

	std::pair<int, int> imagesp_to_screensp(float x, float y);

	void clear_frame_buffer();
	
	void clear_depth_buffer();
	
	inline void point_at(const math::Vec3<float>& point_to, math::Vec3<float>& forward, math::Vec3<float>& up, math::Mat4<float>& pMat4);

	math::Vec3<float> line_plane_intersect(math::Vec3<float>& point, math::Vec3<float>& plane_normal, math::Vec3<float>& line_begin, math::Vec3<float>& line_end);

	int triangle_clip(math::Vec3<float>& point, math::Vec3<float>& plane_normal, Triangle& ref_tri, Triangle& res_tri1, Triangle& res_tri2);
	
	inline void transform_tri(Triangle& t, const math::Mat4<float>& transform);

	inline bool is_visible(Triangle& t);

private:
	Object m_object;

	std::unique_ptr<Camera> m_camera;

	float m_far, m_near, m_fov; 
	float m_cam_angle_x, m_cam_angle_y, m_cam_angle_z;
	float m_cam_movement;

	// transforms
	math::Mat4<float> m_projection, cam_dir, cam_inv;

	// Direction that the light would be pointing in game
	math::Vec3<float> lighting;

	std::vector<Triangle> renderTriangles;

	math::Vec3<float> vUp; // current up vector
	math::Vec3<float> look_dir; // current forward vector aka where the camera is looking
	math::Vec3<float> target; // where the camera is being told to look aka new forward vector
	math::Vec3<float> fVelocity; // velocity of camera in forward direction

	math::Vec3<float> camera_plane;
	Triangle clipped_tris[2]; // holds the new triangles after clipping if any
	int num_clipped = 0; // holds the number of clipped triangles

	unsigned int m_screen_width, m_screen_height, m_buffer_size;
	int m_half_width, m_half_height; // caching these for later
	std::unique_ptr<Pixel> m_frame_buffer;
	std::vector<float> m_depth_buffer;
};
