#include "RadRenderer.h"

RadRenderer::RadRenderer(unsigned int screen_width, unsigned int screen_height, unsigned int buffer_size)

	: m_screen_width(screen_width), m_screen_height(screen_height),
	m_buffer_size(buffer_size)
{
	clear_frame_buffer();

	object = load_obj_file("res/objs/ship.obj");

	/* set up important variables */
	projection_Mat4.set(
		aspect_ratio * scaling_factor, 0.0f, 0.0f, 0.0f,
		0.0f, scaling_factor, 0.0f, 0.0f,
		0.0f, 0.0f, q, 1.0f,
		0.0f, 0.0f, -z_near * q, 0.0f);

	lighting = { 0.0f, 0.0f, -1.0f };

	lighting.normalize();

	// initialize camera position vector
	cam = { 0.0f, 0.0f, 0.0f };
}

Pixel* RadRenderer::update(float elapsed_time)
{
	// clear screen to redraw
	clear_frame_buffer();

	rotate_angle = 0;

	// multiply look_dir by the speed you want to move to get the player's forward velocity
	look_dir.scalar_mul(fVelocity, 8.0f * elapsed_time);

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
		0, 0, 0, 1); // this one rotates player's perspective rather than object

	// set up basis vectors from world origin
	vUp = { 0.0f, 1.0f, 0.0f };
	look_dir = { 0.0f, 0.0f, 1.0f };
	target = { 0.0f, 0.0f, 1.0f };

	camera_plane = { 0.0f, 0.0f, 1.0f };

	// adjust cam based on user input
	//coordinate_projection(target, y_rotation, look_dir); // rotate the previous target vector around the y-axis
	cam.add(look_dir, target);

	// detemine point_at Matrix for next camera position
	point_at(cam, target, vUp, cam_dir);

	cam_inv = look_at(cam_dir);

	// iterate through all triangles in the object
	for (auto o : object)
	{

		// rotate around z-axis
		z_rotation.matmulVec(o.vertices[0], rZ.vertices[0]);
		z_rotation.matmulVec(o.vertices[1], rZ.vertices[1]);
		z_rotation.matmulVec(o.vertices[2], rZ.vertices[2]);

		// rotate around x-axis
		x_rotation.matmulVec(rZ.vertices[0], rX.vertices[0]);
		x_rotation.matmulVec(rZ.vertices[1], rX.vertices[1]);
		x_rotation.matmulVec(rZ.vertices[2], rX.vertices[2]);

		// move object back so it is in view of the camera
		rX.vertices[0].z += 6.0f;
		rX.vertices[1].z += 6.0f;
		rX.vertices[2].z += 6.0f;

		// construct line 1 of the triangle
		l1.x = rX.vertices[1].x - rX.vertices[0].x;
		l1.y = rX.vertices[1].y - rX.vertices[0].y;
		l1.z = rX.vertices[1].z - rX.vertices[0].z;

		// contstruct line 2 of the triangle
		l2.x = rX.vertices[2].x - rX.vertices[1].x;
		l2.y = rX.vertices[2].y - rX.vertices[1].y;
		l2.z = rX.vertices[2].z - rX.vertices[1].z;

		// calculate normal of triangle face
		l1.cross(l2, normal);

		normal.normalize();

		// calculate the angle betwwen the normal and the camera projection to determine if the triangle is visible
		if ((normal.x * rX.vertices[0].x - cam.x
			+ normal.y * rX.vertices[0].y - cam.y
			+ normal.z * rX.vertices[0].z - cam.z) < 0)
		{
			// Dot product is used to determine how direct the light is hitting the traingle to determine what shade it should be
			float lum = normal.dot(lighting);
			Pixel colour = get_colour(lum);

			cam_inv.matmulVec(rX.vertices[0], viewed.vertices[0]);
			cam_inv.matmulVec(rX.vertices[1], viewed.vertices[1]);
			cam_inv.matmulVec(rX.vertices[2], viewed.vertices[2]);

			// before we project the coordinates onto the screen space we need the clipped ones
			num_clipped = triangle_clip(camera_plane, camera_plane, viewed, clipped_tris[0], clipped_tris[1]);

			if (num_clipped > 0) // traingle has been clipped
			{
				for (int c = 0; c < num_clipped; c++)
				{
					// Project all the coordinates to 2D space
					projection_Mat4.matmulVec(clipped_tris[c].vertices[0], pro.vertices[0]);
					projection_Mat4.matmulVec(clipped_tris[c].vertices[1], pro.vertices[1]);
					projection_Mat4.matmulVec(clipped_tris[c].vertices[2], pro.vertices[2]);

					// Center the points and change the scale
					pro.vertices[0].x += 1.0f;
					pro.vertices[0].y += 1.0f;

					pro.vertices[1].x += 1.0f;
					pro.vertices[1].y += 1.0f;

					pro.vertices[2].x += 1.0f;
					pro.vertices[2].y += 1.0f;

					pro.vertices[0].x *= 0.5f * (float)m_screen_width;
					pro.vertices[0].y *= 0.5f * (float)m_screen_height;

					pro.vertices[1].x *= 0.5f * (float)m_screen_width;
					pro.vertices[1].y *= 0.5f * (float)m_screen_height;

					pro.vertices[2].x *= 0.5f * (float)m_screen_width;
					pro.vertices[2].y *= 0.5f * (float)m_screen_height;

					// Assign color to the triangle
					pro.colour = colour;

					renderTriangles.push_back(pro);
				}
			}
			else if (num_clipped == 0) // no new triangles needed
			{
				// Project all the coordinates to 2D space
				projection_Mat4.matmulVec(viewed.vertices[0], pro.vertices[0]);
				projection_Mat4.matmulVec(viewed.vertices[1], pro.vertices[1]);
				projection_Mat4.matmulVec(viewed.vertices[2], pro.vertices[2]);

				// Center the points and change the scale
				pro.vertices[0].x += 1.0f;
				pro.vertices[0].y += 1.0f;

				pro.vertices[1].x += 1.0f;
				pro.vertices[1].y += 1.0f;

				pro.vertices[2].x += 1.0f;
				pro.vertices[2].y += 1.0f;

				pro.vertices[0].x *= 0.5f * (float)m_screen_width;
				pro.vertices[0].y *= 0.5f * (float)m_screen_height;

				pro.vertices[1].x *= 0.5f * (float)m_screen_width;
				pro.vertices[1].y *= 0.5f * (float)m_screen_height;

				pro.vertices[2].x *= 0.5f * (float)m_screen_width;
				pro.vertices[2].y *= 0.5f * (float)m_screen_height;

				// Assign color to the triangle
				pro.colour = colour;

				renderTriangles.push_back(pro);
			}

		}

	}

	// sort triangles by their average z value
	// higher z values mean the traingle should be rendered later aka farther away from front of screen
	std::sort(renderTriangles.begin(), renderTriangles.end(), [](Triangle& tri1, Triangle& tri2) {
		float avg_z1 = (tri1.vertices[0].z + tri1.vertices[1].z + tri1.vertices[2].z) / 3.0f;
		float avg_z2 = (tri2.vertices[0].z + tri2.vertices[1].z + tri2.vertices[2].z) / 3.0f;

		if (avg_z1 > avg_z2)
			return true;
		else
			return false;
		});

	// render all the triangles in order now 
	for (const auto& t : renderTriangles)
	{
		rasterize(t.vertices[0].x, t.vertices[0].y, t.vertices[1].x, t.vertices[1].y, t.vertices[2].x, t.vertices[2].y, t.colour);
	}

	// vector needs to be empty for next redraw
	renderTriangles.clear();

	return m_frame_buffer.get();
}

void RadRenderer::rasterize(int x1, int y1, int x2, int y2, int x3, int y3, Pixel col)
{
	auto SWAP = [](int& x, int& y) { int t = x; x = y; y = t; };
	auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) set_pixel(i, ny, col); };

	int t1x, t2x, y, minx, maxx, t1xp, t2xp;
	bool changed1 = false;
	bool changed2 = false;
	int signx1, signx2, dx1, dy1, dx2, dy2;
	int e1, e2;
	// Sort vertices
	if (y1 > y2) { SWAP(y1, y2); SWAP(x1, x2); }
	if (y1 > y3) { SWAP(y1, y3); SWAP(x1, x3); }
	if (y2 > y3) { SWAP(y2, y3); SWAP(x2, x3); }

	t1x = t2x = x1; y = y1;   // Starting points
	dx1 = (int)(x2 - x1); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
	else signx1 = 1;
	dy1 = (int)(y2 - y1);

	dx2 = (int)(x3 - x1); if (dx2 < 0) { dx2 = -dx2; signx2 = -1; }
	else signx2 = 1;
	dy2 = (int)(y3 - y1);

	if (dy1 > dx1) {   // swap values
		SWAP(dx1, dy1);
		changed1 = true;
	}
	if (dy2 > dx2) {   // swap values
		SWAP(dy2, dx2);
		changed2 = true;
	}

	e2 = (int)(dx2 >> 1);
	// Flat top, just process the second half
	if (y1 == y2) goto next;
	e1 = (int)(dx1 >> 1);

	for (int i = 0; i < dx1;) {
		t1xp = 0; t2xp = 0;
		if (t1x < t2x) { minx = t1x; maxx = t2x; }
		else { minx = t2x; maxx = t1x; }
		// process first line until y value is about to change
		while (i < dx1) {
			i++;
			e1 += dy1;
			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1) t1xp = signx1;//t1x += signx1;
				else          goto next1;
			}
			if (changed1) break;
			else t1x += signx1;
		}
		// Move line
	next1:
		// process second line until y value is about to change
		while (1) {
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp = signx2;//t2x += signx2;
				else          goto next2;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}
	next2:
		if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
		if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
		drawline(minx, maxx, y);    // Draw line from min to max points found on the y
		// Now increase y
		if (!changed1) t1x += signx1;
		t1x += t1xp;
		if (!changed2) t2x += signx2;
		t2x += t2xp;
		y += 1;
		if (y == y2) break;

	}
next:
	// Second half
	dx1 = (int)(x3 - x2); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
	else signx1 = 1;
	dy1 = (int)(y3 - y2);
	t1x = x2;

	if (dy1 > dx1) {   // swap values
		SWAP(dy1, dx1);
		changed1 = true;
	}
	else changed1 = false;

	e1 = (int)(dx1 >> 1);

	for (int i = 0; i <= dx1; i++) {
		t1xp = 0; t2xp = 0;
		if (t1x < t2x) { minx = t1x; maxx = t2x; }
		else { minx = t2x; maxx = t1x; }
		// process first line until y value is about to change
		while (i < dx1) {
			e1 += dy1;
			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1) { t1xp = signx1; break; }//t1x += signx1;
				else          goto next3;
			}
			if (changed1) break;
			else   	   	  t1x += signx1;
			if (i < dx1) i++;
		}
	next3:
		// process second line until y value is about to change
		while (t2x != x3) {
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp = signx2;
				else          goto next4;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}
	next4:

		if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
		if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
		drawline(minx, maxx, y);
		if (!changed1) t1x += signx1;
		t1x += t1xp;
		if (!changed2) t2x += signx2;
		t2x += t2xp;
		y += 1;
		if (y > y3) return;
	}
}

Pixel RadRenderer::get_colour(float lum)
{
	Pixel p = { 255 * lum, 255 * lum, 255 * lum, 255 };
	return p;
}

void RadRenderer::set_pixel(int x, int y, Pixel col)
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

// Takes the current camera position and translates it based on the user input
inline void RadRenderer::point_at(math::Vec3<float>& point_to, math::Vec3<float>& forward, math::Vec3<float>& up, math::Mat4<float>& pMat4)
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
	pMat4.mat[0][0] = nRight.x;			pMat4.mat[0][1] = nRight.y;			pMat4.mat[0][2] = nRight.z;			pMat4.mat[0][3] = 0.0f;
	pMat4.mat[1][0] = nUp.x;			pMat4.mat[1][1] = nUp.y;			pMat4.mat[1][2] = nUp.z;			pMat4.mat[1][3] = 0.0f;
	pMat4.mat[2][0] = nForward.x;		pMat4.mat[2][1] = nForward.y;		pMat4.mat[2][2] = nForward.z;		pMat4.mat[2][3] = 0.0f;
	pMat4.mat[3][0] = point_to.x;		pMat4.mat[3][1] = point_to.y;		pMat4.mat[3][2] = point_to.z;		pMat4.mat[3][3] = 1.0f;
}


math::Mat4<float> RadRenderer::look_at(math::Mat4<float>& pointAt)
{
	// in order to give the appearance of user/camera movement
	// the inverse of the point_at is needed for the transfoemation
	// so that all the other points in the frame get shifted.
	return pointAt.inverse();
}

// returns the point that the given plane and line intersect
math::Vec3<float>& RadRenderer::line_plane_intersect(math::Vec3<float>& point, math::Vec3<float>& plane_normal, math::Vec3<float>& line_begin, math::Vec3<float>& line_end)
{
	// always gotta normalize
	plane_normal.normalize();

	// algorithm to check if line intersects
	// the point where the line intersects plane can be detemined with
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

// this returns the resulting number of traingles after a clip but will sotre those triangles in the res_tri parameters
int RadRenderer::triangle_clip(math::Vec3<float>& point, math::Vec3<float>& plane_normal, Triangle& ref_tri, Triangle& res_tri1, Triangle& res_tri2)
{
	// make sure it's normalized
	plane_normal.normalize();

	// form the equation: x*Nx + y*Ny + z*Nz - N•P = 0
	// where P is a point on the plane and N is a normal vector to the plane
	// can be used to calculate distance of point from a plane
	auto calc_distance = [&](math::Vec3<float>& tri_vertex)
	{
		tri_vertex.normalize();
		return (tri_vertex.x * plane_normal.x + tri_vertex.y * plane_normal.y + tri_vertex.z * plane_normal.z - plane_normal.dot(tri_vertex));
	}; // if 0 then vertex is on the plane, if negative then the vertex is on the outside of the plane (screen edge) else it's on the inside

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

std::vector<Triangle> RadRenderer::load_obj_file(std::string fname)
{
	std::ifstream readFile;
	readFile.open(fname);

	if (!readFile.is_open())
	{
		std::cout << "Cannot open file!";
	}

	math::Vec3<float> vertex;
	std::vector<math::Vec3<float>> vertices;

	std::vector<Triangle> triangles;

	std::string line;

	char startingChar; // Stores the starting character of the line

	int i1, i2, i3; // indexes of the vertices

	// iterate through all lines in file
	while (std::getline(readFile, line))
	{
		std::strstream st;
		st << line;

		// indicates vertex data
		if (line[0] == 'v')
		{
			st >> startingChar >> vertex.x >> vertex.y >> vertex.z;
			vertices.emplace_back(vertex);
		}
		// indicates traingle face data
		else if (line[0] == 'f')
		{
			st >> startingChar >> i1 >> i2 >> i3;
			triangles.push_back(
				{  vertices[i1 - 1], vertices[i2 - 1], vertices[i3 - 1] }
			);
		}
	}

	return triangles;
}