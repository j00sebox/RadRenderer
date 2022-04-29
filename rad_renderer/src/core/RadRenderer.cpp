#include "pch.h"
#include "RadRenderer.h"

#include "mathz/Quaternion.h"

#define DEG_TO_RAD(x) ( ( x / 180.0f ) * 3.14159f  )

RadRenderer::RadRenderer(unsigned int screen_width, unsigned int screen_height, RendererSettings rs)

	: m_screen_width(screen_width), m_screen_height(screen_height),
	m_half_width(screen_width/2), m_half_height(screen_height / 2),
	m_buffer_size(screen_width * screen_height),
	m_near(rs.near), m_far(rs.far),
	m_fov(rs.fov),
	m_object(Object("res/objs/teapot.obj")),
	m_depth_buffer(m_buffer_size, -9999)
{
	clear_frame_buffer();

	m_camera.reset(new Camera());

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

	m_directional_light = { 0.0f, 4.0f, -1.0f };
	m_directional_light.normalize();
}

Pixel* RadRenderer::update(float elapsed_time, float cam_forward, float dx, float dy)
{
	// clear screen to redraw
	clear_frame_buffer();

	m_cam_movement += cam_forward * elapsed_time * 0.001f;

	m_camera->set_pos(mathz::Vec3(0.f, 0.f, m_cam_movement));

	mathz::Quaternion qx(DEG_TO_RAD(dy * elapsed_time * m_rotation_speed), { 1.f, 0.f, 0.f });
	mathz::Quaternion qy(DEG_TO_RAD(dx * elapsed_time * m_rotation_speed), { 0.f, 1.f, 0.f });

	m_object = m_object.get_quaternion() * qx * qy;

	m_object.translate(0.f, -3.f, 6.f);

	// camera transform
	mathz::Mat4 cam_transform = m_camera->get_transform();
	m_view = cam_transform.inverse();

	// iterate through all triangles in the object
	for (auto o : m_object)
	{
		// rotate object
		transform_tri(o, m_object.get_quaternion().convert_to_mat());

		transform_tri(o, m_object.get_transform());

        // convert to camera space
        transform_tri(o, m_view);

		if(out_near_far_bounds(o))
			continue;
        
        // remove when normals are attributes
        o.normal[0] = calculate_normal(o);
        o.normal[1] = o.normal[0];
        o.normal[2] = o.normal[0];
        
		// check if triangle is visible
		if (m_camera->get_forward().dot(o.normal[0]) > 0 && 
			m_camera->get_forward().dot(o.normal[1]) > 0 &&
			m_camera->get_forward().dot(o.normal[2]) > 0)
		{
            o.z[0] = -o.vertices[0].z;
            o.z[1] = -o.vertices[1].z;
            o.z[2] = -o.vertices[2].z;

            transform_tri(o, m_perspective);
            transform_tri(o, m_orthographic);
            
			bool clipped = false;

										// plane point		 // plane normal
			clipped |= clip_triangle({ -1.f,  0.f, 0.f }, {  1.f,  0.f, 0.f }, o);	// left plane
			clipped |= clip_triangle({  1.f,  0.f, 0.f }, { -1.f,  0.f, 0.f }, o);	// right plane
			clipped |= clip_triangle({  0.f,  1.f, 0.f }, {  0.f, -1.f, 0.f }, o);	// top plane
			clipped |= clip_triangle({  0.f, -1.f, 0.f }, {  0.f,  1.f, 0.f }, o);	// bottom plane

			if(!clipped)
				m_render_triangles.push_back(o);
		}
	}
    
    for (const auto& t : m_clipped_tris)
	{
       m_render_triangles.push_back(t);
    }

	for (const auto& t : m_render_triangles)
	{
		rasterize(t);
	}

	// vectors needs to be empty for next redraw
	m_render_triangles.clear();
	m_clipped_tris.clear();
	clear_depth_buffer();

	return m_frame_buffer.get();
}

void RadRenderer::reset_object()
{
	m_object = mathz::Quaternion(1.f, 0.f, 0.f, 0.f);
}

bool out_of_bounds(const Triangle& t)
{
	return  (t.vertices[0].x < -1.f || t.vertices[0].x > 1.f) ||
		(t.vertices[1].x < -1.f || t.vertices[1].x > 1.f) ||
		(t.vertices[2].x < -1.f || t.vertices[2].x > 1.f) ||

		(t.vertices[0].y < -1.f || t.vertices[0].y > 1.f) ||
		(t.vertices[1].y < -1.f || t.vertices[1].y > 1.f) ||
		(t.vertices[2].y < -1.f || t.vertices[2].y > 1.f) ||

		(t.vertices[0].z < -1.f || t.vertices[0].z > 1.f) ||
		(t.vertices[1].z < -1.f || t.vertices[1].z > 1.f) ||
		(t.vertices[2].z < -1.f || t.vertices[2].z > 1.f);
}	

void RadRenderer::rasterize(const Triangle& t)
{
	int min_x, max_x;
	int min_y, max_y;

	mathz::Vec2<int> v0 = { imagesp_to_screensp(t.vertices[0].x, t.vertices[0].y) };
	mathz::Vec2<int> v1 = { imagesp_to_screensp(t.vertices[1].x, t.vertices[1].y) };
	mathz::Vec2<int> v2 = { imagesp_to_screensp(t.vertices[2].x, t.vertices[2].y) };

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
			mathz::Vec2<float> p = { x + 0.5f, y + 0.5f };

			float area0 = edge_function((float)v0.x, (float)v0.y, (float)v1.x, (float)v1.y, p.x, p.y);
			float area1 = edge_function((float)v1.x, (float)v1.y, (float)v2.x, (float)v2.y, p.x, p.y);
			float area2 = edge_function((float)v2.x, (float)v2.y, (float)v0.x, (float)v0.y, p.x, p.y);

			if (area0 >= 0 &&
				area1 >= 0 &&
				area2 >= 0)
			{

				float area_t = edge_function((float)v0.x, (float)v0.y, (float)v1.x, (float)v1.y, (float)v2.x, (float)v2.y);

				// barycentric coordinates
				float l0 = area0 / area_t;
				float l1 = area1 / area_t;
				float l2 = area2 / area_t;

				float int_z = l0 * t.z[0] + l1 * t.z[1] + l2 * t.z[2];

                mathz::Vec3 normal = t.normal[0] * l0 + t.normal[1] * l1 + t.normal[2] * l2; 
                
                float lum = m_directional_light.dot(normal);

				if (int_z > m_depth_buffer[y * m_screen_width + x])
				{
					set_pixel(x, y, get_colour(lum));

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
mathz::Vec3 RadRenderer::line_plane_intersect(mathz::Vec3& point, mathz::Vec3& plane_normal, mathz::Vec3& line_begin, mathz::Vec3& line_end)
{
	// using the equation for a plane Ax + Bx + Cx = D and line P(t) = P + (Q - P) *  t and solving for t
	float t = -(plane_normal.x * (line_begin.x - point.x) + plane_normal.y * (line_begin.y - point.y) + plane_normal.z * (line_begin.z - point.z)) /
		(plane_normal.x * (line_end.x - line_begin.x) + plane_normal.y * (line_end.y - line_begin.y) + plane_normal.z * (line_end.z - line_begin.z));

	mathz::Vec3 intersection_point = line_begin + (line_end - line_begin) * t;

	return intersection_point;
}

bool RadRenderer::clip_triangle(mathz::Vec3&& plane_point, mathz::Vec3&& plane_normal, Triangle& t)
{
	// make sure it's normalized
	plane_normal.normalize();

	int in_verts = 0;
	int out_verts = 0;

	// this will keep track of which vertices are in vs out
	mathz::Vec3 in_vs[3];
	mathz::Vec3 out_vs[3];

	for (const auto& v : t.vertices)
	{
		mathz::Vec3 vert = { v.x, v.y, v.z };
		mathz::Vec3 line = vert - plane_point;
		line.normalize();
		if (plane_normal.dot(line) > 0)
		{
			in_vs[in_verts++] = v;
		}
		else
		{
			out_vs[out_verts++] = v;
		}
	}

	if (in_verts == 2)
	{
		Triangle t1, t2;

		t1.vertices[0] = in_vs[0];
		t1.vertices[1] = in_vs[1];

		t2.vertices[0] = in_vs[1];

		// the intersecting points to the plane will make up the rest of both triangles
		t1.vertices[2] = line_plane_intersect(plane_point, plane_normal, in_vs[0], out_vs[0]);

		t2.vertices[1] = line_plane_intersect(plane_point, plane_normal, in_vs[1], out_vs[0]);
		t2.vertices[2] = t1.vertices[2]; // both new triangles share this vertex

		// copy over attributes
		t1.z[0] = t.z[0];
		t1.z[1] = t.z[1];
		t1.z[2] = t.z[2];

		t1.normal[0] = t.normal[0];
		t1.normal[1] = t.normal[1];
		t1.normal[2] = t.normal[2];

		t2.z[0] = t.z[0];
		t2.z[1] = t.z[1];
		t2.z[2] = t.z[2];

		t2.normal[0] = t.normal[0];
		t2.normal[1] = t.normal[1];
		t2.normal[2] = t.normal[2];

		m_clipped_tris.push_back(t1);
		m_clipped_tris.push_back(t2);
		
		return true;
	}
	else if (in_verts == 1)
	{
		Triangle t1;
		t1.vertices[0] = in_vs[0];

		t1.vertices[1] = line_plane_intersect(plane_point, plane_normal, in_vs[0], out_vs[0]);
		t1.vertices[2] = line_plane_intersect(plane_point, plane_normal, in_vs[0], out_vs[1]);

		t1.z[0] = t.z[0];
		t1.z[1] = t.z[1];
		t1.z[2] = t.z[2];

		t1.normal[0] = t.normal[0];
		t1.normal[1] = t.normal[1];
		t1.normal[2] = t.normal[2];

		m_clipped_tris.push_back(t1);

		return true;
	}
	else if (in_verts == 0)
	{
		return true;
	}
    
    return false;
}

bool RadRenderer::out_near_far_bounds(const Triangle& t)
{
	return	(t.vertices[0].z < m_near || t.vertices[0].z > m_far) ||
			(t.vertices[0].z < m_near || t.vertices[0].z > m_far) ||
			(t.vertices[0].z < m_near || t.vertices[0].z > m_far);
}

void RadRenderer::transform_tri(Triangle& t, const mathz::Mat4& transform)
{
	t.vertices[0] = transform * t.vertices[0];
	t.vertices[1] = transform * t.vertices[1];
	t.vertices[2] = transform * t.vertices[2];
}

mathz::Vec3 RadRenderer::calculate_normal(Triangle& t)
{
	// construct line 1 of the triangle
	mathz::Vec3 l0 = {
		t.vertices[1].x - t.vertices[0].x,
		t.vertices[1].y - t.vertices[0].y,
		t.vertices[1].z - t.vertices[0].z
	};

	// construct line 2 of the triangle
	mathz::Vec3 l1 = {
		t.vertices[2].x - t.vertices[0].x,
		t.vertices[2].y - t.vertices[0].y,
		t.vertices[2].z - t.vertices[0].z
	};
    
    mathz::Vec3 face_normal = l1.cross(l0);
	face_normal.normalize();
    
    return face_normal;
}
