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
    bool clip_triangle(math::Vec3<float>&& point, math::Vec3<float>&& plane_normal, Triangle& t);
	inline void transform_tri(Triangle& t, const math::Mat4<float>& transform);
	inline math::Vec3<float> calculate_normal(Triangle& t);

private:
	Object m_object;

	std::unique_ptr<Camera> m_camera;

	float m_far, m_near, m_fov;
	float m_angle_x = 0.f, m_angle_y = 0.f, m_angle_z = 0.f;
	float m_rotation_speed = 0.001f;
	float m_cam_movement = 0.f;

	// transforms
	math::Mat4<float> m_view, m_perspective, m_orthographic;
    
    // lighting
	math::Vec3<float> m_directional_light;
    float m_diffuse_constant = 0.75f; // make some kind of material class for this

	std::vector<Triangle> m_render_triangles;
	std::vector<Triangle> m_clipped_tris;
    
    // screen stuff
	unsigned int m_screen_width, m_screen_height, m_buffer_size;
	int m_half_width, m_half_height; // caching these for later
	std::unique_ptr<Pixel> m_frame_buffer;
	std::vector<float> m_depth_buffer;
};
