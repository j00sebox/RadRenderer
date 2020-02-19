#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <strstream>

/*
 - Library provided from https://github.com/OneLoneCoder
 - Used to draw shapes to the console 
*/
#include "olcConsoleGameEngine.h"

#include "Matrix.h"

#define CUBE_DEMO

class Triangle
{
public:

	Triangle() : vertices(3)
	{	
		std::vector<float> init{ 0.0f, 0.0f, 0.0f };
		vertices[0] = init;
		vertices[1] = init;
		vertices[2] = init;
	}

	Triangle(std::vector< std::vector<float > > v) : vertices(3)
	{
		vertices[0] = v[0];
		vertices[1] = v[1];
		vertices[2] = v[2];
	}

	std::vector<Vector3D> vertices;

	wchar_t symbol;
	short colour;
};

std::vector<Triangle> cube_demo()
{
	return {

		// south
		Triangle({ {0.0f, 0.0f, 0.0f},    {0.0f, 1.0f, 0.0f},    {1.0f, 1.0f, 0.0f} }),
		Triangle({ {0.0f, 0.0f, 0.0f},    {1.0f, 1.0f, 0.0f},    {1.0f, 0.0f, 0.0f} }),

		// east                                                      
		Triangle({ {1.0f, 0.0f, 0.0f},    {1.0f, 1.0f, 0.0f},    {1.0f, 1.0f, 1.0f} }),
		Triangle({ {1.0f, 0.0f, 0.0f},    {1.0f, 1.0f, 1.0f},    {1.0f, 0.0f, 1.0f} }),

		// north                                                     
		Triangle({ {1.0f, 0.0f, 1.0f},    {1.0f, 1.0f, 1.0f},    {0.0f, 1.0f, 1.0f} }),
		Triangle({ {1.0f, 0.0f, 1.0f},    {0.0f, 1.0f, 1.0f},    {0.0f, 0.0f, 1.0f} }),

		// west                                                      
		Triangle({ {0.0f, 0.0f, 1.0f},    {0.0f, 1.0f, 1.0f},    {0.0f, 1.0f, 0.0f} }),
		Triangle({ {0.0f, 0.0f, 1.0f},    {0.0f, 1.0f, 0.0f},    {0.0f, 0.0f, 0.0f} }),

		// top                                                       
		Triangle({ {0.0f, 1.0f, 0.0f},    {0.0f, 1.0f, 1.0f},    {1.0f, 1.0f, 1.0f} }),
		Triangle({ {0.0f, 1.0f, 0.0f},    {1.0f, 1.0f, 1.0f},    {1.0f, 1.0f, 0.0f} }),

		// bottom                                                    
		Triangle({ {1.0f, 0.0f, 1.0f},    {0.0f, 0.0f, 1.0f},    {0.0f, 0.0f, 0.0f} }),
		Triangle({ {1.0f, 0.0f, 1.0f},    {0.0f, 0.0f, 0.0f},    {1.0f, 0.0f, 0.0f} }),

	};
	
}

Matrix projection_matrix(4, 4);

Matrix x_rotation(4, 4);

Matrix z_rotation(4, 4);

Matrix tempVec(1, 4);


class EmazingEngine : public olcConsoleGameEngine
{
public:
	EmazingEngine() {}

	bool OnUserCreate() override
	{
#ifdef CUBE_DEMO
		object = cube_demo();	
#else
		object = LoadOBJFile("3DTestObjects/teapot.obj");
#endif  CUBE_DEMO

		//projection_matrix.set(4, 4);
		projection_matrix = { { aspect_ratio * scaling_factor, 0.0f, 0.0f, 0.0f }, { 0.0f, scaling_factor, 0.0f, 0.0f }, { 0.0f, 0.0f, q, 1.0f } , { 0.0f, 0.0f, -z_near * q, 0.0f } };

		lighting = { 0.0f, 0.0f, -1.0f };

		lighting.normalize();

		cam = { 0.0f, 0.0f, 0.0f };

		return true;

	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear screen to redraw
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

		//rotate_angle += 1.0f * fElapsedTime;

		if (GetKey(VK_UP).bHeld)
			cam.y -= 8.0f * fElapsedTime;	// Travel Upwards

		if (GetKey(VK_DOWN).bHeld)
			cam.y += 8.0f * fElapsedTime;	// Travel Downwards

		if (GetKey(VK_LEFT).bHeld)
			cam.x -= 8.0f * fElapsedTime;	// Travel Along X-Axis

		if (GetKey(VK_RIGHT).bHeld)
			cam.x += 8.0f * fElapsedTime;	// Travel Along X-Axis
		
		x_rotation = { {1, 0, 0, 0}, {0, cosf(rotate_angle * 0.5f), sinf(rotate_angle * 0.5f), 0}, {0, -sinf(rotate_angle * 0.5f), cosf(rotate_angle * 0.5f), 0}, {0, 0, 0, 1} };
		z_rotation = { {cosf(rotate_angle), sinf(rotate_angle), 0, 0}, {-sinf(rotate_angle), cosf(rotate_angle), 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1} };

		/*vUp = { 0,1,0 };
		look_dir = { 0,0,1 };
		cam.add(look_dir, forward);*/

		/*Matrix cameraM = point_at(cam, forward, vUp);

		Matrix cameraInv = look_at(cameraM);*/

		for (auto o : object)
		{
			
			// Rotate around z-axis
			coordinate_projection(o.vertices[0], z_rotation, rZ.vertices[0]);
			coordinate_projection(o.vertices[1], z_rotation, rZ.vertices[1]);
			coordinate_projection(o.vertices[2], z_rotation, rZ.vertices[2]);

			// Rotate around x-axis
			coordinate_projection(rZ.vertices[0], x_rotation, rX.vertices[0]);
			coordinate_projection(rZ.vertices[1], x_rotation, rX.vertices[1]);
			coordinate_projection(rZ.vertices[2], x_rotation, rX.vertices[2]);

			// move object back so it is in view of the camera
			rX.vertices[0].z += 3.0f;
			rX.vertices[1].z += 3.0f;
			rX.vertices[2].z += 3.0f;

			// Construct line 1 of the triangle
			l1.x = rX.vertices[1].x - rX.vertices[0].x;
			l1.y = rX.vertices[1].y - rX.vertices[0].y;
			l1.z = rX.vertices[1].z - rX.vertices[0].z;

			// Contstruct line 2 of the triangle
			l2.x = rX.vertices[2].x - rX.vertices[0].x;
			l2.y = rX.vertices[2].y - rX.vertices[0].y;
			l2.z = rX.vertices[2].z - rX.vertices[0].z;

			// calculate normal of triangle face
			l1.cross(l2, normal);

			normal.normalize();

			// Calculate the angle betwwen the normal and the camera projection to determine if the triangle is visible
			if ( (normal.x * ( rX.vertices[0].x - cam.x )
				+ normal.y * ( rX.vertices[0].y - cam.y )
				+ normal.z * ( rX.vertices[0].z - cam.z ) ) < 0.0f )
			{
				// Dot product is used to determine how direct the light is hitting the traingle to determine what shade it should be
				float dotProd = normal.dot(lighting);

				// The larger dot product in this case means the more lit up the triangle face will be 
				CHAR_INFO colour = GetColour(dotProd);

				/*coordinate_projection(rX.vertices[0], cameraInv, viewed.vertices[0]);
				coordinate_projection(rX.vertices[1], cameraInv, viewed.vertices[1]);
				coordinate_projection(rX.vertices[2], cameraInv, viewed.vertices[2]);*/
				
				// Project all the coordinates to 2D space
				coordinate_projection(rX.vertices[0], projection_matrix, pro.vertices[0]);
				coordinate_projection(rX.vertices[1], projection_matrix, pro.vertices[1]);
				coordinate_projection(rX.vertices[2], projection_matrix, pro.vertices[2]);

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
			FillTriangle(t.vertices[0].x, t.vertices[0].y, t.vertices[1].x, t.vertices[1].y, t.vertices[2].x, t.vertices[2].y, t.symbol, t.colour);
		}
		
		// vector needs to be empty for next redraw
		renderTriangles.clear();

		return true;
	}

	// Map 3D coordinates to 2D space
	inline void coordinate_projection(Vector3D& vertex, Matrix& operation, Vector3D& outVec)
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

	//inline void coordinate_projection(Vector3D& vertex, Matrix& operation, Vector3D& outVec)
	//{
	//	/*outVec.x = vertex.x * operation.m[0][0] + vertex.y * operation.m[1][0] + vertex.z * operation.m[2][0] + operation.m[3][0];
	//	outVec.y = vertex.x * operation.m[0][1] + vertex.y * operation.m[1][1] + vertex.z * operation.m[2][1] + operation.m[3][1];
	//	outVec.z = vertex.x * operation.m[0][2] + vertex.y * operation.m[1][2] + vertex.z * operation.m[2][2] + operation.m[3][2];

	//	float leftOver = vertex.x * operation.m[0][3] + vertex.y * operation.m[1][3] + vertex.z * operation.m[2][3] + operation.m[3][3];*/

	//	tempVec = { { vertex.x, vertex.y, vertex.z, 1.0f } };

	//	Matrix res = tempVec * operation;

	//	res = res / res(0, 3);

	//	res.clip();

	//	outVec.value = res.value;

	//	// Dived entire matrix by the last value to convert it back to 3D space
	//	// Also satistfies dividing the terms by z

	//	/*if (leftOver != 0.0f)
	//	{
	//		outVec.x /= leftOver;
	//		outVec.y /= leftOver;
	//		outVec.z /= leftOver;
	//	}*/

	//}

	/*inline Matrix point_at(Vector3D& point_to, Vector3D& forward, Vector3D& up)
	{
		Vector3D nForward;
		forward.subtract(point_to, nForward);
		nForward.normalize();

		Vector3D temp = nForward.multiply(up.dot(nForward));
		Vector3D nUp; 
		up.subtract(temp, nUp);
		nUp.normalize();

		Vector3D nRight;
		nUp.cross(nForward, nRight);

		Matrix matrix;
		matrix.m[0][0] = nRight.x;		matrix.m[0][1] = nRight.y;		matrix.m[0][2] = nRight.z;		matrix.m[0][3] = 0.0f;
		matrix.m[1][0] = nUp.x;			matrix.m[1][1] = nUp.y;			matrix.m[1][2] = nUp.z;			matrix.m[1][3] = 0.0f;
		matrix.m[2][0] = nForward.x;	matrix.m[2][1] = nForward.y;	matrix.m[2][2] = nForward.z;	matrix.m[2][3] = 0.0f;
		matrix.m[3][0] = point_to.x;	matrix.m[3][1] = point_to.y;	matrix.m[3][2] = point_to.z;	matrix.m[3][3] = 1.0f;

		return matrix;
	}

	inline Matrix look_at(Matrix& pointAt)
	{
		Matrix matrix;
		matrix.m[0][0] = pointAt.m[0][0]; matrix.m[0][1] = pointAt.m[1][0]; matrix.m[0][2] = pointAt.m[2][0]; matrix.m[0][3] = 0.0f;
		matrix.m[1][0] = pointAt.m[0][1]; matrix.m[1][1] = pointAt.m[1][1]; matrix.m[1][2] = pointAt.m[2][1]; matrix.m[1][3] = 0.0f;
		matrix.m[2][0] = pointAt.m[0][2]; matrix.m[2][1] = pointAt.m[1][2]; matrix.m[2][2] = pointAt.m[2][2]; matrix.m[2][3] = 0.0f;
		matrix.m[3][0] = -(pointAt.m[3][0] * matrix.m[0][0] + pointAt.m[3][1] * matrix.m[1][0] + pointAt.m[3][2] * matrix.m[2][0]);
		matrix.m[3][1] = -(pointAt.m[3][0] * matrix.m[0][1] + pointAt.m[3][1] * matrix.m[1][1] + pointAt.m[3][2] * matrix.m[2][1]);
		matrix.m[3][2] = -(pointAt.m[3][0] * matrix.m[0][2] + pointAt.m[3][1] * matrix.m[1][2] + pointAt.m[3][2] * matrix.m[2][2]);
		matrix.m[3][3] = 1.0f;
		return matrix;
	}*/

	// Loads vertex and face data from txt file realting to obj file
	std::vector<Triangle> LoadOBJFile(std::string fname)
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

	

private:
	std::vector<Triangle> object;
	float z_far = 1000.0f; // represents the distance from the theoretical distance in the screen to the users face
	float z_near = 0.1f; // represents the distance from the users face to the screen
	float q = z_far / (z_far - z_near);

	float theta = 90.0f; // the field of view for the player
	float thetaRAD = theta / 180.0f * 3.14159f;

	float scaling_factor = 1.0f / tanf(thetaRAD * 0.5f); // amount needed to scale coordinates based on the fov
	float aspect_ratio = (float)ScreenHeight() / (float)ScreenWidth();

	float rotate_angle; // perodically changing to give the appearance that the object is rotating

	// Projection matrices
	Triangle pro;

	Triangle rX;

	Triangle rZ;

	Triangle viewed;

	// Vectors to calculate the normal of triangle faces.
	Vector3D l1;
	Vector3D l2;
	Vector3D normal;

	// Position of camera aka user's view
	Vector3D cam;

	// Direction that the light would be pointing in game
	Vector3D lighting;

	std::vector<Triangle> renderTriangles;

	Vector3D vUp;
	Vector3D look_dir;
	Vector3D forward;
	
};

int main()
{
	EmazingEngine game;
	if (game.ConstructConsole(250, 250, 4, 4))
		game.Start();

	/*Vector3D testVec;
	std::vector<float> t{ 17.0f, 2.0f, 1.0f };
	testVec.assign(t);

	testVec.value[0][0] = 4.0f;

	std::cout << testVec.x;*/

	/*Matrix test(3, 2);
	Matrix oTest(3, 3);

	test = { {4.0f, 6.0f}, {7.0f, 8.0f}, {19.0f, 2.0f} };
	oTest = { {13.0f, 12.0f, 5.0f}, {3.0f, 0.0f, 2.0f}, {17.0f, 2.0f, 1.0f} };

	Matrix res = oTest * test;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 2; j++)
			std::cout << res.value[i][j] << " ";

		std::cout << std::endl;
	}*/
		
	return 0;
}
