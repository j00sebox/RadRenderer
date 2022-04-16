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

	float edge_function(float x0, float y0, float x1, float y1, float x2, float y2);

	Pixel get_colour(float lum);

	void set_pixel(int x, int y, const Pixel& col);

	std::pair<int, int> imagesp_to_screensp(float x, float y);

	void clear_frame_buffer();

	void clear_depth_buffer();

	math::Vec3<float> line_plane_intersect(math::Vec3<float>& point, math::Vec3<float>& plane_normal, math::Vec3<float>& line_begin, math::Vec3<float>& line_end);

	int triangle_clip(math::Vec3<float>& point, math::Vec3<float>& plane_normal, Triangle& ref_tri, Triangle& res_tri1, Triangle& res_tri2);

	inline void transform_tri(Triangle& t, const math::Mat4<float>& transform);

	inline math::Vec3<float> calculate_normal(Triangle& t);

private:
	Object m_object;

	std::unique_ptr<Camera> m_camera;

	float m_far, m_near, m_fov;
	float m_angle_x, m_angle_y, m_angle_z;
	float m_cam_movement;

	// transforms
	math::Mat4<float> m_view, m_perspective, m_orthographic;

	math::Vec3<float> m_directional_light;
    float m_diffuse_constant = 0.75f;

	std::vector<Triangle> m_render_triangles;

	math::Vec3<float> camera_plane;
	Triangle clipped_tris[2]; // holds the new triangles after clipping if any
	int num_clipped = 0; // holds the number of clipped triangles

	unsigned int m_screen_width, m_screen_height, m_buffer_size;
	int m_half_width, m_half_height; // caching these for later
	std::unique_ptr<Pixel> m_frame_buffer;
	std::vector<float> m_depth_buffer;
};
