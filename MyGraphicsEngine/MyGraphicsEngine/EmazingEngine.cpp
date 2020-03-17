#include "EmazingEngine.h"

Triangle::Triangle() : vertices(3)
{
	std::vector<float> init{ 0.0f, 0.0f, 0.0f };
	vertices[0] = init;
	vertices[1] = init;
	vertices[2] = init;
}

Triangle::Triangle(std::vector< std::vector<float > > v) : vertices(3)
{
	vertices[0] = v[0];
	vertices[1] = v[1];
	vertices[2] = v[2];
}

std::vector<Triangle> cube_demo()
{
	std::vector<Triangle> test_cube;

	// south
	test_cube.push_back(Triangle({ {0.0f, 0.0f, 0.0f},	  {0.0f, 1.0f, 0.0f},    {1.0f, 1.0f, 0.0f} }));
	test_cube.push_back(Triangle({ {0.0f, 0.0f, 0.0f},    {1.0f, 1.0f, 0.0f},    {1.0f, 0.0f, 0.0f} }));

	test_cube.push_back(Triangle({ {1.0f, 0.0f, 0.0f},    {1.0f, 1.0f, 0.0f},    {1.0f, 1.0f, 1.0f} }));
	test_cube.push_back(Triangle({ {1.0f, 0.0f, 0.0f},    {1.0f, 1.0f, 1.0f},    {1.0f, 0.0f, 1.0f} }));

	test_cube.push_back(Triangle({ {1.0f, 0.0f, 1.0f},    {1.0f, 1.0f, 1.0f},    {0.0f, 1.0f, 1.0f} }));
	test_cube.push_back(Triangle({ {1.0f, 0.0f, 1.0f},    {0.0f, 1.0f, 1.0f},    {0.0f, 0.0f, 1.0f} }));

	test_cube.push_back(Triangle({ {0.0f, 0.0f, 1.0f},    {0.0f, 1.0f, 1.0f},    {0.0f, 1.0f, 0.0f} }));
	test_cube.push_back(Triangle({ {0.0f, 0.0f, 1.0f},    {0.0f, 1.0f, 0.0f},    {0.0f, 0.0f, 0.0f} }));

	test_cube.push_back(Triangle({ {0.0f, 1.0f, 0.0f},    {0.0f, 1.0f, 1.0f},    {1.0f, 1.0f, 1.0f} }));
	test_cube.push_back(Triangle({ {0.0f, 1.0f, 0.0f},    {1.0f, 1.0f, 1.0f},    {1.0f, 1.0f, 0.0f} }));

	test_cube.push_back(Triangle({ {1.0f, 0.0f, 1.0f},    {0.0f, 0.0f, 1.0f},    {0.0f, 0.0f, 0.0f} }));
	test_cube.push_back(Triangle({ {1.0f, 0.0f, 1.0f},    {0.0f, 0.0f, 0.0f},    {1.0f, 0.0f, 0.0f} }));

	return test_cube;
}

inline void EmazingEngine::coordinate_projection(Vector3D& vertex, Matrix& operation, Vector3D& outVec)
{
	outVec.x = vertex.x * operation.value[0][0] + vertex.y * operation.value[1][0] + vertex.z * operation.value[2][0] + operation.value[3][0];
	outVec.y = vertex.x * operation.value[0][1] + vertex.y * operation.value[1][1] + vertex.z * operation.value[2][1] + operation.value[3][1];
	outVec.z = vertex.x * operation.value[0][2] + vertex.y * operation.value[1][2] + vertex.z * operation.value[2][2] + operation.value[3][2];

	float leftOver = vertex.x * operation.value[0][3] + vertex.y * operation.value[1][3] + vertex.z * operation.value[2][3] + operation.value[3][3];

	// Dived entire matrix by the last value to convert it back to 3D space
	// Also satistfies dividing the terms by z
	if (leftOver != 0.0f)
	{
		outVec.x /= leftOver;
		outVec.y /= leftOver;
		outVec.z /= leftOver;
	}

}

//inline void EmazingEngine::coordinate_projection(Vector3D& vertex, Matrix& operation, Vector3D& outVec)
//{
//	Matrix tempVec(1, 4);
//
//	tempVec = { { vertex.x, vertex.y, vertex.z, 1.0f } };
//
//	Matrix res = tempVec * operation;
//
//	res = res / res(0, 3);
//
//	res.clip();
//
//	outVec = res;
//
//}

// Takes the current camera position and translates it based on the user input
inline void EmazingEngine::point_at(Vector3D& point_to, Vector3D& forward, Vector3D& up, Matrix& matrix)
{
	// The new forward vector aka where the camera is pointing
	Vector3D nForward;
	forward.subtract(point_to, nForward);
	nForward.normalize();

	// Calculate the up vector in relation to the new camera direction
	Vector3D temp;
	nForward.scalar_mul(temp, (up.dot(nForward))); // scalar is used to calculate how far the nUp vector is displaced
	Vector3D nUp;
	up.subtract(temp, nUp);
	nUp.normalize();

	// Then the vector pointing to the right of the camera is perpendicular to the 2 new ones calculated
	Vector3D nRight;
	nUp.cross(nForward, nRight);

	// Set up camera direction matrix
	matrix.value[0][0] = nRight.x;		matrix.value[0][1] = nRight.y;		matrix.value[0][2] = nRight.z;		matrix.value[0][3] = 0.0f;
	matrix.value[1][0] = nUp.x;			matrix.value[1][1] = nUp.y;			matrix.value[1][2] = nUp.z;			matrix.value[1][3] = 0.0f;
	matrix.value[2][0] = nForward.x;	matrix.value[2][1] = nForward.y;	matrix.value[2][2] = nForward.z;	matrix.value[2][3] = 0.0f;
	matrix.value[3][0] = point_to.x;	matrix.value[3][1] = point_to.y;	matrix.value[3][2] = point_to.z;	matrix.value[3][3] = 1.0f;
}

inline Matrix EmazingEngine::look_at(Matrix& pointAt)
{
	// in order to give the appearance of user/camera movement
	// the inverse of the point_at is needed for the transfoemation
	// so that all the other points in the frame get shifted.
	return pointAt.inverse();
}


bool EmazingEngine::OnUserCreate()
{
#ifdef CUBE_DEMO
	object = cube_demo();
#else
	object = LoadOBJFile("3DTestObjects/teapot.obj");
#endif  CUBE_DEMO

	/* set up important variables */
	projection_matrix = { { aspect_ratio * scaling_factor, 0.0f, 0.0f, 0.0f }, { 0.0f, scaling_factor, 0.0f, 0.0f }, { 0.0f, 0.0f, q, 1.0f } , { 0.0f, 0.0f, -z_near * q, 0.0f } };

	lighting = { 0.0f, 0.0f, -1.0f };

	lighting.normalize();

	// initialize camera position vector
	cam = { 0.0f, 0.0f, 0.0f };
	
	return true;

}

bool EmazingEngine::OnUserUpdate(float fElapsedTime)
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
	x_rotation = { {1, 0, 0, 0}, {0, cosf(rotate_angle * 0.5f), sinf(rotate_angle * 0.5f), 0}, {0, -sinf(rotate_angle * 0.5f), cosf(rotate_angle * 0.5f), 0}, {0, 0, 0, 1} };
	z_rotation = { {cosf(rotate_angle), sinf(rotate_angle), 0, 0}, {-sinf(rotate_angle), cosf(rotate_angle), 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1} };
	y_rotation = { {cosf(facing_dir), 0, sinf(facing_dir), 0}, {0, 0, 1, 0}, {-sinf(facing_dir),  0, cosf(facing_dir), 0}, {0, 0, 0, 1} }; // this one rotates player's perspective rather than object

	// set uo basis vectors
	vUp = { 0.0f, 1.0f, 0.0f };
	look_dir = { 0.0f, 0.0f, 1.0f };
	target = { 0.0f, 0.0f, 1.0f };

	// adjust cam based on user input
	coordinate_projection(target, y_rotation, look_dir); // rotate the previous target vector around the y-axis
	cam.add(look_dir, target);

	point_at(cam, target, vUp, cam_dir);

	cam_inv = look_at(cam_dir);

	// iterate through all triangles in the object
	for (auto o : object)
	{

		// rotate around z-axis
		coordinate_projection(o.vertices[0], z_rotation, rZ.vertices[0]);
		coordinate_projection(o.vertices[1], z_rotation, rZ.vertices[1]);
		coordinate_projection(o.vertices[2], z_rotation, rZ.vertices[2]);

		// rotate around x-axis
		coordinate_projection(rZ.vertices[0], x_rotation, rX.vertices[0]);
		coordinate_projection(rZ.vertices[1], x_rotation, rX.vertices[1]);
		coordinate_projection(rZ.vertices[2], x_rotation, rX.vertices[2]);

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

			coordinate_projection(rX.vertices[0], cam_inv, viewed.vertices[0]);
			coordinate_projection(rX.vertices[1], cam_inv, viewed.vertices[1]);
			coordinate_projection(rX.vertices[2], cam_inv, viewed.vertices[2]);

			// Project all the coordinates to 2D space
			coordinate_projection(viewed.vertices[0], projection_matrix, pro.vertices[0]);
			coordinate_projection(viewed.vertices[1], projection_matrix, pro.vertices[1]);
			coordinate_projection(viewed.vertices[2], projection_matrix, pro.vertices[2]);

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

	// sort triangles by their average z value
	// higher z values mean the traingle should be rendered later
	std::sort(renderTriangles.begin(), renderTriangles.end(), [](Triangle& tri1, Triangle& tri2) {
		float avg_z1 = (tri1.vertices[0].z + tri1.vertices[1].z + tri1.vertices[2].z) / 3.0f;
		float avg_z2 = (tri2.vertices[0].z + tri2.vertices[1].z + tri2.vertices[2].z) / 3.0f;

		if (avg_z1 > avg_z2)
			return true;
		else
			return false;
		});

	// render all the triangles in order now 
	for (auto t : renderTriangles)
	{
		FillTriangle(t.vertices[0](0, 0), t.vertices[0](0, 1), t.vertices[1](0, 0), t.vertices[1](0, 1), t.vertices[2](0, 0), t.vertices[2](0, 1), t.symbol, t.colour);
	}


	// vector needs to be empty for next redraw
	renderTriangles.clear();

	return true;
}

std::vector<Triangle> EmazingEngine::LoadOBJFile(std::string fname)
{
	std::ifstream readFile;
	readFile.open(fname);

	if (!readFile.is_open())
	{
		std::cout << "Cannot open file!";
	}

	Vector3D vertex;
	std::vector<Vector3D> vertices;

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
			vertices.push_back(vertex);
		}
		// indicates traingle face data
		else if (line[0] == 'f')
		{
			st >> startingChar >> i1 >> i2 >> i3;
			triangles.push_back(
				Triangle({ vertices[i1 - 1].value[0], vertices[i2 - 1].value[0], vertices[i3 - 1].value[0] })
			);
		}
	}

	return triangles;
}