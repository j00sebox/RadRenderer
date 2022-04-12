#include "RadRenderer.h"

#include <iostream>
#include <algorithm>

#define DEG_TO_RAD(x) ( x / 180.0f * 3.14159f  )

RadRenderer::RadRenderer(unsigned int screen_width, unsigned int screen_height, RendererSettings rs)

	: m_screen_width(screen_width), m_screen_height(screen_height),
	m_buffer_size(screen_width * screen_height),
	m_near(rs.near), m_far(rs.far),
	m_fov(rs.fov),
#ifdef PLATFORM_LINUX
	m_object(Object("./radrenderer/res/objs/teapot.obj")),
#elif PLATFORM_WINDOWS
	m_object(Object("res/objs/teapot.obj")),
#endif
	m_depth_buffer(m_buffer_size, -9999),
	rotate_angle(0.f)
{
	clear_frame_buffer();

	float m_fovRAD = DEG_TO_RAD(m_fov);
	float scaling_factor = 1.0f / tanf(m_fovRAD * 0.5f);
	float aspect_ratio = (float)m_screen_height / (float)m_screen_width;

	float q = m_far / (m_far - m_near);

	// create projection matrix
	m_projection.set(
		aspect_ratio * scaling_factor, 0.0f, 0.0f, 0.0f,
		0.0f, scaling_factor, 0.0f, 0.0f,
		0.0f, 0.0f, q, -1.0f,
		0.0f, 0.0f, 2 * m_near * q, 0.0f);

	lighting = { 0.0f, 0.0f, -1.0f };

	lighting.normalize();

	// set up basis vectors from world origin
	vUp = { 0.0f, 1.0f, 0.0f };
	look_dir = { 0.0f, 0.0f, 1.0f };
	target = { 0.0f, 0.0f, 1.0f };
	camera_plane = { 0.0f, 0.0f, 1.0f };

	m_camera = { 0.0f, 0.0f, 0.0f };
}

Pixel* RadRenderer::update(float elapsed_time)
{
	// clear screen to redraw
	clear_frame_buffer();

	rotate_angle += 1.0f * elapsed_time * 0.001f;

	// update rotation matrices
	x_rotation.set(
		1, 0, 0, 0,
		0, cosf(rotate_angle * 0.5f), sinf(rotate_angle * 0.5f), 0,
		0, -sinf(rotate_angle * 0.5f), cosf(rotate_angle * 0.5f), 0,
		0, 0, 0, 1);
	z_rotation.set(
		cosf(rotate_angle), sinf(rotate_angle), 0, 0,
		-sinf(rotate_angle), cosf(rotate_angle), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
	y_rotation.set(
		cosf(facing_dir), 0, sinf(facing_dir), 0,
		0, 0, 1, 0,
		-sinf(facing_dir), 0, cosf(facing_dir), 0,
		0, 0, 0, 1);

	// camera transform
	point_at(m_camera, target, vUp, cam_dir);

	cam_inv = cam_dir.inverse();

	// iterate through all triangles in the object
	for (auto o : m_object)
	{
		// rotate around z-axis
		transform_tri(o, z_rotation);

		// rotate around x-axis
		transform_tri(o, x_rotation);

		// move object back so it is in view of the camera
		o.vertices[0].z += 6.0f;
		o.vertices[1].z += 6.0f;
		o.vertices[2].z += 6.0f;

		// construct line 1 of the triangle
		l1.x = o.vertices[1].x - o.vertices[0].x;
		l1.y = o.vertices[1].y - o.vertices[0].y;
		l1.z = o.vertices[1].z - o.vertices[0].z;

		// contstruct line 2 of the triangle
		l2.x = o.vertices[2].x - o.vertices[1].x;
		l2.y = o.vertices[2].y - o.vertices[1].y;
		l2.z = o.vertices[2].z - o.vertices[1].z;

		// calculate normal of triangle face
		l1.cross(l2, normal);

		normal.normalize();

		// check if triangle is visible
		if ( look_dir.dot(normal) < 0 )
		{
			float lum = normal.dot(lighting);
			Pixel colour = get_colour(lum);

			transform_tri(o, cam_inv);

			// before we project the coordinates onto the screen space we need the clipped ones
			num_clipped = triangle_clip(camera_plane, camera_plane, o, clipped_tris[0], clipped_tris[1]);

			if (num_clipped > 0)
			{
				for (int c = 0; c < num_clipped; c++)
				{
					// Convert all coordinates to projection space
					transform_tri(clipped_tris[c], m_projection);

					// Center the points and change the scale
					clipped_tris[c].vertices[0].x += 1.0f;
					clipped_tris[c].vertices[0].y += 1.0f;

					clipped_tris[c].vertices[1].x += 1.0f;
					clipped_tris[c].vertices[1].y += 1.0f;

					clipped_tris[c].vertices[2].x += 1.0f;
					clipped_tris[c].vertices[2].y += 1.0f;

					clipped_tris[c].vertices[0].x *= 0.5f * (float)m_screen_width;
					clipped_tris[c].vertices[0].y *= 0.5f * (float)m_screen_height;

					clipped_tris[c].vertices[1].x *= 0.5f * (float)m_screen_width;
					clipped_tris[c].vertices[1].y *= 0.5f * (float)m_screen_height;

					clipped_tris[c].vertices[2].x *= 0.5f * (float)m_screen_width;
					clipped_tris[c].vertices[2].y *= 0.5f * (float)m_screen_height;

					clipped_tris[c].colour = colour;

					renderTriangles.push_back(clipped_tris[c]);
				}
			}
			else if (num_clipped == 0)
			{
				transform_tri(o, m_projection);

				// Center the points and change the scale
				o.vertices[0].x += 1.0f;
				o.vertices[0].y += 1.0f;

				o.vertices[1].x += 1.0f;
				o.vertices[1].y += 1.0f;

				o.vertices[2].x += 1.0f;
				o.vertices[2].y += 1.0f;

				o.vertices[0].x *= 0.5f * (float)m_screen_width;
				o.vertices[0].y *= 0.5f * (float)m_screen_height;

				o.vertices[1].x *= 0.5f * (float)m_screen_width;
				o.vertices[1].y *= 0.5f * (float)m_screen_height;

				o.vertices[2].x *= 0.5f * (float)m_screen_width;
				o.vertices[2].y *= 0.5f * (float)m_screen_height;

				o.colour = colour;

				renderTriangles.push_back(o);
			}

		}

	}

	for (const auto& t : renderTriangles)
	{
		rasterize(t);
	}

	// vector needs to be empty for next redraw
	renderTriangles.clear();
	clear_depth_buffer();

	return m_frame_buffer.get();
}

void RadRenderer::rasterize(const Triangle& t)
{
	int min_x, max_x;
	int min_y, max_y;

	// bounding box
	min_x = std::min(t.vertices[0].x, t.vertices[1].x);
	min_x = std::min(min_x, (int)t.vertices[2].x);

	max_x = std::max(t.vertices[0].x, t.vertices[1].x);
	max_x = std::max(max_x, (int)t.vertices[2].x);

	min_y = std::min(t.vertices[0].y, t.vertices[1].y);
	min_y = std::min(min_y, (int)t.vertices[2].y);

	max_y = std::max(t.vertices[0].y, t.vertices[1].y);
	max_y = std::max(max_y, (int)t.vertices[2].y);

	for (int y = min_y; y < max_y; y++)
	{
		for (int x = min_x; x < max_x; x++)
		{

			math::Vec2<float> p = { x + 0.5f, y + 0.5f };

			if (edge_function(t.vertices[0].x, t.vertices[0].y, t.vertices[1].x, t.vertices[1].y, p) &&
				edge_function(t.vertices[1].x, t.vertices[1].y, t.vertices[2].x, t.vertices[2].y, p) &&
				edge_function(t.vertices[2].x, t.vertices[2].y, t.vertices[0].x, t.vertices[0].y, p))
			{
				float avg_z = (t.vertices[0].z + t.vertices[1].z + t.vertices[2].z) / 3.0f;

				if (x >= 0 && x < m_screen_width && y >= 0 && y < m_screen_height)
				{
					if (avg_z > m_depth_buffer[y * m_screen_width + x])
					{
						set_pixel(x, y, t.colour);

						m_depth_buffer[y * m_screen_width + x] = avg_z;
					}
				}
			}

		}
	}
}

bool RadRenderer::edge_function(float x1, float y1, float x2, float y2, const math::Vec2<float>& p)
{
	int res = (p.x - x1) * (y2 - y1) - (p.y - y1) * (x2 - x1);
	return (res > 0);
}

Pixel RadRenderer::get_colour(float lum)
{
	Pixel p = { (std::uint8_t)(255 * lum), (std::uint8_t)(255 * lum), (std::uint8_t)(255 * lum), 255 };
	return p;
}

void RadRenderer::set_pixel(int x, int y, const Pixel& col)
{
	if (x >= 0 && x < m_screen_width && y >= 0 && y < m_screen_height)
	{
		m_frame_buffer.get()[y * m_screen_width + x] = col;
	}
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

// Takes the current camera position and translates it based on the user input
inline void RadRenderer::point_at(const math::Vec3<float>& point_to, math::Vec3<float>& forward, math::Vec3<float>& up, math::Mat4<float>& mat)
{
	// The new forward vector aka where the camera is pointing
	math::Vec3<float> nForward;
	forward.subtract(point_to, nForward);
	nForward.normalize();

	// Calculate the up vector in relation to the new camera direction
	math::Vec3<float> temp;
	nForward.scalar_mul(temp, (up.dot(nForward))); // scalar is used to calculate how far the nUp vector is displaced
	math::Vec3<float> nUp;
	up.subtract(temp, nUp);
	nUp.normalize();

	// Then the vector pointing to the right of the camera is perpendicular to the 2 new ones calculated
	math::Vec3<float> nRight;
	nUp.cross(nForward, nRight);

	// Set up camera direction Mat4
	mat(0, 0) = nRight.x;		mat(0, 1) = nRight.y;		mat(0, 2) = nRight.z;		mat(0, 3) = 0.0f;
	mat(1, 0) = nUp.x;			mat(1, 1) = nUp.y;			mat(1, 2) = nUp.z;			mat(1, 3) = 0.0f;
	mat(2, 0) = nForward.x;		mat(2, 1) = nForward.y;		mat(2, 2) = nForward.z;		mat(2, 3) = 0.0f;
	mat(3, 0) = point_to.x;		mat(3, 1) = point_to.y;		mat(3, 2) = point_to.z;		mat(3, 3) = 1.0f;
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

// this returns the resulting number of traingles after a clip but will sort those triangles in the res_tri parameters
int RadRenderer::triangle_clip(math::Vec3<float>& point, math::Vec3<float>& plane_normal, Triangle& ref_tri, Triangle& res_tri1, Triangle& res_tri2)
{
	// make sure it's normalized
	plane_normal.normalize();

	// from the equation: x*Nx + y*Ny + z*Nz - N�P = 0
	auto calc_distance = [&](math::Vec3<float>& tri_vertex)
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
		res_tri1.colour = ref_tri.colour;

		res_tri2.colour = ref_tri.colour;

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
		res_tri1.colour = ref_tri.colour;

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

void RadRenderer::transform_tri(Triangle& tri, const math::Mat4<float>& transform)
{
	transform.mat_mul_vec(tri.vertices[0]);
	transform.mat_mul_vec(tri.vertices[1]);
	transform.mat_mul_vec(tri.vertices[2]);
}
