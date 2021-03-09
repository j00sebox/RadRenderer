#include "RadRenderer.h"

#define TEAPOT 1

// Takes the current camera position and translates it based on the user input
inline void RadRenderer::point_at(Vector3D<float>& point_to, Vector3D<float>& forward, Vector3D<float>& up, Matrix4x4<float>& pMatrix4x4)
{
	// The new forward vector aka where the camera is pointing
	Vector3D<float> nForward;
	forward.subtract(point_to, nForward);
	nForward.normalize();

	// Calculate the up vector in relation to the new camera direction
	Vector3D<float> temp;
	nForward.scalar_mul(temp, (up.dot(nForward))); // scalar is used to calculate how far the nUp vector is displaced
	Vector3D<float> nUp;
	up.subtract(temp, nUp);
	nUp.normalize();

	// Then the vector pointing to the right of the camera is perpendicular to the 2 new ones calculated
	Vector3D<float> nRight;
	nUp.cross(nForward, nRight);

	// Set up camera direction Matrix4x4
	pMatrix4x4.mat[0][0] = nRight.x;		pMatrix4x4.mat[0][1] = nRight.y;		pMatrix4x4.mat[0][2] = nRight.z;		pMatrix4x4.mat[0][3] = 0.0f;
	pMatrix4x4.mat[1][0] = nUp.x;			pMatrix4x4.mat[1][1] = nUp.y;			pMatrix4x4.mat[1][2] = nUp.z;			pMatrix4x4.mat[1][3] = 0.0f;
	pMatrix4x4.mat[2][0] = nForward.x;		pMatrix4x4.mat[2][1] = nForward.y;		pMatrix4x4.mat[2][2] = nForward.z;		pMatrix4x4.mat[2][3] = 0.0f;
	pMatrix4x4.mat[3][0] = point_to.x;		pMatrix4x4.mat[3][1] = point_to.y;		pMatrix4x4.mat[3][2] = point_to.z;		pMatrix4x4.mat[3][3] = 1.0f;
}


Matrix4x4<float> RadRenderer::look_at(Matrix4x4<float>& pointAt)
{
	// in order to give the appearance of user/camera movement
	// the inverse of the point_at is needed for the transfoemation
	// so that all the other points in the frame get shifted.
	return pointAt.inverse();
}

// returns the point that the given plane and line intersect
Vector3D<float>& RadRenderer::line_plane_intersect(Vector3D<float>& point, Vector3D<float>& plane_normal, Vector3D<float>& line_begin, Vector3D<float>& line_end)
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
	Vector3D<float> line;
	line_end.subtract(line_begin, line);

	Vector3D<float> intersection;
	line.scalar_mul(intersection, t);

	line_begin.add(intersection, line);

	return line;
}

// this returns the resulting number of traingles after a clip but will sotre those triangles in the res_tri parameters
int RadRenderer::triangle_clip(Vector3D<float>& point, Vector3D<float>& plane_normal, Triangle& ref_tri, Triangle& res_tri1, Triangle& res_tri2)
{
	// make sure it's normalized
	plane_normal.normalize();

	// form the equation: x*Nx + y*Ny + z*Nz - N•P = 0
	// where P is a point on the plane and N is a normal vector to the plane
	// can be used to calculate distance of point from a plane
	auto calc_distance = [&](Vector3D<float>& tri_vertex)
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
	Vector3D<float> in_vs[3];
	Vector3D<float> out_vs[3];


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
		res_tri1.symbol = ref_tri.symbol;

		res_tri2.colour = ref_tri.colour;
		res_tri2.symbol = ref_tri.symbol;

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
		res_tri1.symbol = ref_tri.symbol;

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


bool RadRenderer::OnUserCreate()
{
#if TEAPOT == 1
	object = LoadOBJFile("3DTestObjects/teapot.obj");
#else
	object = LoadOBJFile("3DTestObjects/ship.obj");
#endif 

	/* set up important variables */
	projection_Matrix4x4.set( 
		aspect_ratio * scaling_factor, 0.0f, 0.0f, 0.0f,
		0.0f, scaling_factor, 0.0f, 0.0f,
		0.0f, 0.0f, q, 1.0f,
		0.0f, 0.0f, -z_near * q, 0.0f);

	lighting = { 0.0f, 0.0f, -1.0f };

	lighting.normalize();

	// initialize camera position vector
	cam = { 0.0f, 0.0f, 0.0f };
	
	return true;

}

bool RadRenderer::OnUserUpdate(float fElapsedTime)
{
	// clear screen to redraw
	Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

	rotate_angle += 1.0f * fElapsedTime;

	// multiply look_dir by the speed you want to move to get the player's forward velocity
	look_dir.scalar_mul(fVelocity, 8.0f * fElapsedTime);

	if (GetKey(VK_UP).bHeld)
		cam.y -= 8.0f * fElapsedTime;	// go up

	if (GetKey(VK_DOWN).bHeld)
		cam.y += 8.0f * fElapsedTime;	// go down

	if (GetKey(VK_LEFT).bHeld)
		cam.x -= 8.0f * fElapsedTime;	// go left along x-axis

	if (GetKey(VK_RIGHT).bHeld)
		cam.x += 8.0f * fElapsedTime;	// go right along x-axis

	if (GetKey(L'W').bHeld)
		cam.add(fVelocity, cam);	    // go forwards

	if (GetKey(L'S').bHeld)
		cam.subtract(fVelocity, cam);   // go backwards

	if (GetKey(L'A').bHeld)
		facing_dir -= 2.0f * fElapsedTime; // rotate camera left

	if (GetKey(L'D').bHeld)
		facing_dir += 2.0f * fElapsedTime; // rotate camera right
	

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
		-sinf(facing_dir),  0, cosf(facing_dir), 0,
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
			float dotProd = normal.dot(lighting);

			// The larger dot product in this case means the more lit up the triangle face will be 
			CHAR_INFO colour = GetColour(dotProd);


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
					projection_Matrix4x4.matmulVec(clipped_tris[c].vertices[0], pro.vertices[0]);
					projection_Matrix4x4.matmulVec(clipped_tris[c].vertices[1], pro.vertices[1]);
					projection_Matrix4x4.matmulVec(clipped_tris[c].vertices[2], pro.vertices[2]);

					// Center the points and change the scale
					pro.vertices[0].x += 1.0f;
					pro.vertices[0].y += 1.0f;

					pro.vertices[1].x += 1.0f;
					pro.vertices[1].y += 1.0f;

					pro.vertices[2].x += 1.0f;
					pro.vertices[2].y += 1.0f;

					pro.vertices[0].x *= 0.5f * (float)ScreenWidth();
					pro.vertices[0].y *= 0.5f * (float)ScreenHeight();

					pro.vertices[1].x *= 0.5f * (float)ScreenWidth();
					pro.vertices[1].y *= 0.5f * (float)ScreenHeight();

					pro.vertices[2].x *= 0.5f * (float)ScreenWidth();
					pro.vertices[2].y *= 0.5f * (float)ScreenHeight();

					// Assign color and character info to the triangle
					pro.symbol = colour.Char.UnicodeChar;
					pro.colour = colour.Attributes;

					renderTriangles.push_back(pro);
				}
			}
			else if (num_clipped == 0) // no new triangles needed
			{
				// Project all the coordinates to 2D space
				projection_Matrix4x4.matmulVec(viewed.vertices[0], pro.vertices[0]);
				projection_Matrix4x4.matmulVec(viewed.vertices[1], pro.vertices[1]);
				projection_Matrix4x4.matmulVec(viewed.vertices[2], pro.vertices[2]);

				// Center the points and change the scale
				pro.vertices[0].x += 1.0f;
				pro.vertices[0].y += 1.0f;

				pro.vertices[1].x += 1.0f;
				pro.vertices[1].y += 1.0f;

				pro.vertices[2].x += 1.0f;
				pro.vertices[2].y += 1.0f;

				pro.vertices[0].x *= 0.5f * (float)ScreenWidth();
				pro.vertices[0].y *= 0.5f * (float)ScreenHeight();

				pro.vertices[1].x *= 0.5f * (float)ScreenWidth();
				pro.vertices[1].y *= 0.5f * (float)ScreenHeight();

				pro.vertices[2].x *= 0.5f * (float)ScreenWidth();
				pro.vertices[2].y *= 0.5f * (float)ScreenHeight();

				// Assign color and character info to the triangle
				pro.symbol = colour.Char.UnicodeChar;
				pro.colour = colour.Attributes;

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
		FillTriangle(t.vertices[0].x, t.vertices[0].y, t.vertices[1].x, t.vertices[1].y, t.vertices[2].x, t.vertices[2].y, t.symbol, t.colour);
	}

	// vector needs to be empty for next redraw
	renderTriangles.clear();

	return true;
}

std::vector<Triangle> RadRenderer::LoadOBJFile(std::string fname)
{
	std::ifstream readFile;
	readFile.open(fname);

	if (!readFile.is_open())
	{
		std::cout << "Cannot open file!";
	}

	Vector3D<float> vertex;
	std::vector< Vector3D<float> > vertices;

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