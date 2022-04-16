#include "pch.h"
#include "RadRenderer.h"

#define DEG_TO_RAD(x) ( x / 180.0f * 3.14159f  )

RadRenderer::RadRenderer(unsigned int screen_width, unsigned int screen_height, RendererSettings rs)

	: m_screen_width(screen_width), m_screen_height(screen_height),
	m_half_width(screen_width/2), m_half_height(screen_height / 2),
	m_buffer_size(screen_width * screen_height),
	m_near(rs.near), m_far(rs.far),
	m_fov(rs.fov),
	m_object(Object("res/objs/teapot.obj")),
	m_depth_buffer(m_buffer_size, -9999),
	m_cam_movement(0.f),
	m_angle_x(0.f), m_angle_y(0.f), m_angle_z(0.f),
	m_camera(new Camera())
{
	clear_frame_buffer();

	float m_fovRAD = DEG_TO_RAD(m_fov);
	float scaling_factor = 1.0f / tanf(m_fov * 0.5f);
	float aspect_ratio = (float)m_screen_height / (float)m_screen_width;

	float q = m_far / (m_far - m_near);

	// create projection matrix
	m_perspective.set(
		aspect_ratio * scaling_factor,	0.f,			0.f,			0.f,
		0.f,							scaling_factor, 0.f,			0.f,
		0.f,							0.f,			q,				1.f,
		0.f,							0.f,			-m_near * q,	0.f
	);

	m_orthographic.set(
		1.f,	0.f,	0.f,						0.f,
		0.f,	1.f,	0.f,						0.f,
		0.f,	0.f,	1.f / (m_far - m_near),		-m_near / (m_far - m_near),
		0.f,	0.f,	0.f,						1.f
	);

	m_directional_light = { 0.0f, 6.0f, -1.0f };
	camera_plane = { 0.0f, 0.0f, 1.0f };

	m_directional_light.normalize();
}

Pixel* RadRenderer::update(float elapsed_time, float cam_forward, float rotate_x, float rotate_y)
{
	// clear screen to redraw
	clear_frame_buffer();

	m_angle_x += rotate_x * elapsed_time * 0.001f;
	m_angle_y += rotate_y * elapsed_time * 0.001f;

	m_cam_movement += cam_forward * elapsed_time * 0.001f;

	m_camera->set_pos(math::Vec3<float>(0.f, 0.f, m_cam_movement));

	m_object.rotate_x(m_angle_x);
	m_object.rotate_y(m_angle_y);

	m_object.translate(0.f, -2.f, 6.f);

	// camera transform
	math::Mat4<float> cam_transform = m_camera->get_transform();
	m_view = cam_transform.inverse();
    
    std::cout << m_view;

	// iterate through all triangles in the object
	for (auto o : m_object)
	{
		transform_tri(o, m_object.get_transform());
        
        // convert to camera space
        transform_tri(o, m_view);
            
		calculate_normal(o);

		// check if triangle is visible
		if (m_camera->get_forward().dot(o.normal[0]) > 0 &&
			m_camera->get_forward().dot(o.normal[1]) > 0 &&
			m_camera->get_forward().dot(o.normal[2]) > 0)
		{
            float lum0 = m_directional_light.dot(o.normal[0]);
            float lum1 = m_directional_light.dot(o.normal[1]);
            float lum2 = m_directional_light.dot(o.normal[2]);
        
			Pixel colour0 = get_colour(lum0);
			Pixel colour1 = get_colour(lum1);
			Pixel colour2 = get_colour(lum2);

			if( (o.vertices[0].z < m_near || o.vertices[0].z > m_far) ||
                (o.vertices[1].z < m_near || o.vertices[1].z > m_far) ||
                (o.vertices[2].z < m_near || o.vertices[2].z > m_far) )
					continue;

            o.z[0] = -o.vertices[0].z;
            o.z[1] = -o.vertices[1].z;
            o.z[2] = -o.vertices[2].z;

            transform_tri(o, m_perspective);
            transform_tri(o, m_orthographic);
            
            if( (o.vertices[0].x < -1.f || o.vertices[0].x > 1.f) ||
                (o.vertices[1].x < -1.f || o.vertices[1].x > 1.f) ||
                (o.vertices[2].x < -1.f || o.vertices[2].x > 1.f) ||
                
                (o.vertices[0].y < -1.f || o.vertices[0].y > 1.f) ||
                (o.vertices[1].y < -1.f || o.vertices[1].y > 1.f) ||
                (o.vertices[2].y < -1.f || o.vertices[2].y > 1.f) )
					continue;

            o.colours[0] = colour0;
            o.colours[1] = colour1;
            o.colours[2] = colour2;

            m_render_triangles.push_back(o);
			
		}
	}

	for (const auto& t : m_render_triangles)
	{
		rasterize(t);
	}

	// vector needs to be empty for next redraw
	m_render_triangles.clear();
	clear_depth_buffer();

	m_object.reset_transform();

	return m_frame_buffer.get();
}

void RadRenderer::rasterize(const Triangle& t)
{
	int min_x, max_x;
	int min_y, max_y;

	math::Vec2<int> v0 = { imagesp_to_screensp(t.vertices[0].x, t.vertices[0].y) };
	math::Vec2<int> v1 = { imagesp_to_screensp(t.vertices[1].x, t.vertices[1].y) };
	math::Vec2<int> v2 = { imagesp_to_screensp(t.vertices[2].x, t.vertices[2].y) };

	// bounding box
	min_x = std::min(v0.x, v1.x);
	min_x = std::min(min_x, v2.x);

	max_x = std::max(v0.x, v1.x);
	max_x = std::max(max_x, v2.x);

	min_y = std::min(v0.y, v1.y);
	min_y = std::min(min_y, v2.y);

	max_y = std::max(v0.y, v1.y);
	max_y = std::max(max_y, v2.y);

	for (int y = min_y; y < max_y; y++)
	{
		for (int x = min_x; x < max_x; x++)
		{
			math::Vec2<float> p = { x + 0.5f, y + 0.5f };

			float area_t = edge_function(v0.x, v0.y, v1.x, v1.y, v2.x, v2.y);

			float area0 = edge_function(v0.x, v0.y, v1.x, v1.y, p.x, p.y);
			float area1 = edge_function(v1.x, v1.y, v2.x, v2.y, p.x, p.y);
			float area2 = edge_function(v2.x, v2.y, v0.x, v0.y, p.x, p.y);

			if (area0 >= 0 &&
				area1 >= 0 &&
				area2 >= 0)
			{
				// barycentric coordinates
				float l0 = area0 / area_t;
				float l1 = area1 / area_t;
				float l2 = area2 / area_t;

				float int_z = l0 * t.z[0] + l1 * t.z[1] + l2 * t.z[2];

				Pixel int_c;
				int_c.r = l0 * t.colours[0].r + l1 * t.colours[1].r + l2 * t.colours[2].r;
				int_c.g = l0 * t.colours[0].g + l1 * t.colours[1].g + l2 * t.colours[2].g;
				int_c.b = l0 * t.colours[0].b + l1 * t.colours[1].b + l2 * t.colours[2].b;
				int_c.a = 255;
                

				if (int_z > m_depth_buffer[y * m_screen_width + x])
				{
					set_pixel(x, y, int_c);

					m_depth_buffer[y * m_screen_width + x] = int_z;
				}

			}
		}
	}
}

float RadRenderer::edge_function(float x0, float y0, float x1, float y1, float x2, float y2)
{
	return ( (x2 - x0) * (y1 - y0) - (y2 - y0) * (x1 - x0) );
}

Pixel RadRenderer::get_colour(float lum)
{
	Pixel p = { (std::uint8_t)(255 * cosf(lum) * m_diffuse_constant), (std::uint8_t)(255 * cosf(lum) * m_diffuse_constant), (std::uint8_t)(255 * cosf(lum) * m_diffuse_constant), 255 };
	return p;
}

void RadRenderer::set_pixel(int x, int y, const Pixel& col)
{
	m_frame_buffer.get()[y * m_screen_width + x] = col;
}

std::pair<int, int> RadRenderer::imagesp_to_screensp(float x, float y)
{
	x = m_half_width * x + m_half_width;
	y = m_half_height * y + m_half_height;

	return { (int)x, (int)y };
}

void RadRenderer::clear_frame_buffer()
{
	m_frame_buffer.reset(new Pixel[m_buffer_size]);
}

void RadRenderer::clear_depth_buffer()
{
	for (auto& f : m_depth_buffer)
		f = -9999;
}

// returns the point that the given plane and line intersect
math::Vec3<float> RadRenderer::line_plane_intersect(math::Vec3<float>& point, math::Vec3<float>& plane_normal, math::Vec3<float>& line_begin, math::Vec3<float>& line_end)
{
	// always gotta normalize
	plane_normal.normalize();

	// line_begin + line*t = p0 + p1*u + p2*v
	float np = -point.dot(plane_normal);
	float dotb = line_begin.dot(plane_normal);
	float dote = line_end.dot(plane_normal);
	float t = (-np - dotb) / (dote - dotb);

	// construct line
	math::Vec3<float> line;
	line_end.subtract(line_begin, line);

	math::Vec3<float> intersection;
	line.scalar_mul(intersection, t);

	line_begin.add(intersection, line);

	return line;
}

// this returns the resulting number of traingles after a clip
int RadRenderer::triangle_clip(math::Vec3<float>& point, math::Vec3<float>& plane_normal, Triangle& ref_tri, Triangle& res_tri1, Triangle& res_tri2)
{
	// make sure it's normalized
	plane_normal.normalize();

	// from the equation: x*Nx + y*Ny + z*Nz - NP = 0
	auto calc_distance = [plane_normal](math::Vec3<float> tri_vertex)
	{
		tri_vertex.normalize();
		return (tri_vertex.x * plane_normal.x + tri_vertex.y * plane_normal.y + tri_vertex.z * plane_normal.z - plane_normal.dot(tri_vertex));
	};

	// calculate distance for each vertex
	float d0 = calc_distance(ref_tri.vertices[0]);
	float d1 = calc_distance(ref_tri.vertices[1]);
	float d2 = calc_distance(ref_tri.vertices[2]);

	int in_verts = 0;
	int out_verts = 0;

	// this will keep track of which vertices are in vs out
	math::Vec3<float> in_vs[3];
	math::Vec3<float> out_vs[3];

	// determine amount of inside and outside vertices
	if (d0 >= 0) { in_vs[in_verts] = ref_tri.vertices[0]; in_verts++; }
	else { out_vs[out_verts] = ref_tri.vertices[0]; out_verts++; }

	if (d1 >= 0) { in_vs[in_verts] = ref_tri.vertices[1]; in_verts++; }
	else { out_vs[out_verts] = ref_tri.vertices[1]; out_verts++; }

	if (d2 >= 0) { in_vs[in_verts] = ref_tri.vertices[2]; in_verts++; }
	else { out_vs[out_verts] = ref_tri.vertices[2]; out_verts++; }

	// determine how many traingles are needed
	if (in_verts == 3)
		return 0; // no new traingles are needed
	else if (in_verts == 2)
	{
		// both new traingles will have the same properties as the one being clipped
		/*res_tri1.colours = ref_tri.colours;

		res_tri2.colours = ref_tri.colours;*/

		// first new triangle contains the two in vertices
		res_tri1.vertices[0] = in_vs[0];
		res_tri1.vertices[1] = in_vs[1];

		// second one will just contain the second vertex as it's first
		res_tri2.vertices[0] = in_vs[1];

		// the intersecting points to the plane will make up the rest of both triangles
		res_tri1.vertices[2] = line_plane_intersect(point, plane_normal, out_vs[0], in_vs[0]);

		res_tri2.vertices[1] = line_plane_intersect(point, plane_normal, out_vs[0], in_vs[1]);
		res_tri2.vertices[2] = res_tri1.vertices[2]; // both new triangles share this vertex

		return 2;
	}
	else if (in_verts == 1)
	{
		// new triangle will have same properties as old one
		//res_tri1.colour = ref_tri.colour;

		// just 1 new triangle but smaller
		res_tri1.vertices[0] = in_vs[0];

		// 2 new vertices are made by finding the intersection to the plane ( screen edge )
		res_tri1.vertices[1] = line_plane_intersect(point, plane_normal, out_vs[0], in_vs[0]);
		res_tri1.vertices[2] = line_plane_intersect(point, plane_normal, out_vs[1], in_vs[0]);

		return 1;
	}
	else if (in_verts == 0)
		return -1; // triangle lies outside of screen so it should be discarded
}

void RadRenderer::transform_tri(Triangle& t, const math::Mat4<float>& transform)
{
	transform.mat_mul_vec(t.vertices[0]);
	transform.mat_mul_vec(t.vertices[1]);
	transform.mat_mul_vec(t.vertices[2]);
}

inline void RadRenderer::calculate_normal(Triangle& t)
{
	// construct line 1 of the triangle
	math::Vec3<float> l0 = {
		t.vertices[1].x - t.vertices[0].x,
		t.vertices[1].y - t.vertices[0].y,
		t.vertices[1].z - t.vertices[0].z
	};

	// contstruct line 2 of the triangle
	math::Vec3<float> l1 = {
		t.vertices[2].x - t.vertices[0].x,
		t.vertices[2].y - t.vertices[0].y,
		t.vertices[2].z - t.vertices[0].z
	};

	l1.cross(l0, t.normal[0]);
	t.normal[0].normalize();

	l0 = {
		t.vertices[2].x - t.vertices[1].x,
		t.vertices[2].y - t.vertices[1].y,
		t.vertices[2].z - t.vertices[1].z
	};

	l1 = {
		t.vertices[0].x - t.vertices[1].x,
		t.vertices[0].y - t.vertices[1].y,
		t.vertices[0].z - t.vertices[1].z
	};

	l1.cross(l0, t.normal[1]);
	t.normal[1].normalize();

	l0 = {
		t.vertices[0].x - t.vertices[2].x,
		t.vertices[0].y - t.vertices[2].y,
		t.vertices[0].z - t.vertices[2].z
	};

	l1 = {
		t.vertices[1].x - t.vertices[2].x,
		t.vertices[1].y - t.vertices[2].y,
		t.vertices[1].z - t.vertices[2].z
	};

	l1.cross(l0, t.normal[2]);
	t.normal[2].normalize();
}
