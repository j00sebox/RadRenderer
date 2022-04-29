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

	void reset_object();

protected:
	void rasterize(const Triangle& t);
	float edge_function(float x0, float y0, float x1, float y1, float x2, float y2);
	Pixel get_colour(float lum);
	void set_pixel(int x, int y, const Pixel& col);
	std::pair<int, int> imagesp_to_screensp(float x, float y);
	void clear_frame_buffer();
	void clear_depth_buffer();
	mathz::Vec3 line_plane_intersect(mathz::Vec3& point, mathz::Vec3& plane_normal, mathz::Vec3& line_begin, mathz::Vec3& line_end);
    bool clip_triangle(mathz::Vec3&& point, mathz::Vec3&& plane_normal, Triangle& t);
	bool out_near_far_bounds(const Triangle& t);
	inline void transform_tri(Triangle& t, const mathz::Mat4& transform);
	inline mathz::Vec3 calculate_normal(Triangle& t);

private:
	Object m_object;

	std::unique_ptr<Camera> m_camera;

	float m_far, m_near, m_fov;
	float m_angle_x = 0.f, m_angle_y = 0.f, m_angle_z = 0.f;
	float m_rotation_speed = 0.001f;
	float m_cam_movement = 0.f;

	// transforms
	mathz::Mat4 m_view, m_perspective, m_orthographic;
    
    // lighting
	mathz::Vec3 m_directional_light;
    float m_diffuse_constant = 0.75f; // make some kind of material class for this

	std::vector<Triangle> m_render_triangles;
	std::vector<Triangle> m_clipped_tris;
    
    // screen stuff
	unsigned int m_screen_width, m_screen_height, m_buffer_size;
	int m_half_width, m_half_height; // caching these for later
	std::unique_ptr<Pixel> m_frame_buffer;
	std::vector<float> m_depth_buffer;
};
