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

//#define CUBE_DEMO

class Triangle
{
public:
	
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
	return
	{
		Triangle({ { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f } }),
		Triangle({ { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } }),

		Triangle({ { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } }),
		Triangle({ { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f } }),

		Triangle({ { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 1.0f } }),
		Triangle({ { 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } }),

		Triangle({ { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } }),
		Triangle({ { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }),

		Triangle({ { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } }),
		Triangle({ { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f } }),

		Triangle({ { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } }),
		Triangle({ { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } }),
	};
}

Matrix projection_matrix(4, 4);
Matrix x_rotation(4, 4);
Matrix z_rotation(4, 4);

class EmazingEngine : public olcConsoleGameEngine
{
public:
	bool OnUserCreate() override
	{
#ifdef CUBE_DEMO
		object = cube_demo();	
#else
		object = LoadOBJFile("3DTestObjects/teapot.obj");
#endif  CUBE_DEMO

		projection_matrix.Assign({ { aspect_ratio * scaling_factor, 0.0f, 0.0f, 0.0f }, { 0.0f, scaling_factor, 0.0f, 0.0f }, { 0.0f, 0.0f, q, 1.0f } , { 0.0f, 0.0f, -z_near * q, 0.0f } });
		
		return true;

	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear screen to redraw
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

		rotate_angle += 1.0f * fElapsedTime;

		x_rotation.Assign({ {1, 0, 0, 0}, {0, cosf(rotate_angle * 0.5f), sinf(rotate_angle * 0.5f), 0}, {0, -sinf(rotate_angle * 0.5f), cosf(rotate_angle * 0.5f), 0}, {0, 0, 0, 1} });
		z_rotation.Assign({ {cosf(rotate_angle), sinf(rotate_angle), 0, 0}, {-sinf(rotate_angle), cosf(rotate_angle), 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1} });

		// Position of camera aka user's view
		Vector3D cam;
		cam.x = 0.0f; cam.y = 0.0f; cam.z = 0.0f;

		// Direction that the light would be pointing in game
		Vector3D lighting;
		lighting.x = 0.0f; lighting.y = 0.0f; lighting.z = -1.0f;//.Assign({ {0.0f, 0.0f, -1.0f} });

		lighting.normalize();

		for (auto o : object)
		{
			// Rotate around z-axis
			coordinate_projection(o.vertices[0], z_rotation, rZ1);
			coordinate_projection(o.vertices[1], z_rotation, rZ2);
			coordinate_projection(o.vertices[2], z_rotation, rZ3);

			// Rotate around x-axis
			coordinate_projection(rZ1, x_rotation, rX1);
			coordinate_projection(rZ2, x_rotation, rX2);
			coordinate_projection(rZ3, x_rotation, rX3);

			// move object back so it is in view of the camera
			rX1.z += 8.0f;
			rX2.z += 8.0f;
			rX3.z += 8.0f;

			// Construct line 1 of the triangle
			l1.x = rX2.x - rX1.x;
			l1.y = rX2.y - rX1.y;
			l1.z = rX2.z - rX1.z;

			// Contstruct line 2 of the triangle
			l2.x = rX3.x - rX1.x;
			l2.y = rX3.y - rX1.y;
			l2.z = rX3.z - rX1.z;

			// calculate normal of triangle face
			l1.cross(l2, normal);

			normal.normalize();

			// Calculate the angle betwwen the normal and the camera projection to determine if the triangle is visible
			if ( (normal.x * ( rX1.x - cam.x )
				+ normal.y * ( rX1.y - cam.y )
				+ normal.z * ( rX1.z - cam.z ) ) < 0.0f )
			{
				// Dot product is used to determine how direct the light is hitting the traingle to determine what shade it should be
				float dotProd = normal.dot(lighting);

				// The larger dot product in this case means the more lit up the triangle face will be 
				CHAR_INFO colour = GetColour(dotProd);
				
				// Project all the coordinates to 2D space
				coordinate_projection(rX1, projection_matrix, pro1);
				coordinate_projection(rX2, projection_matrix, pro2);
				coordinate_projection(rX3, projection_matrix, pro3);

				// Center the points and change the scale
				pro1.x += 1.0f;
				pro1.y += 1.0f;

				pro2.x += 1.0f;
				pro2.y += 1.0f;

				pro3.x += 1.0f;
				pro3.y += 1.0f;

				pro1.x *= 0.5f * (float)ScreenWidth();
				pro1.y *= 0.5f * (float)ScreenHeight();

				pro2.x *= 0.5f * (float)ScreenWidth();
				pro2.y *= 0.5f * (float)ScreenHeight();

				pro3.x *= 0.5f * (float)ScreenWidth();
				pro3.y *= 0.5f * (float)ScreenHeight();

				Triangle tri({ {pro1.x, pro1.y, pro1.z}, {pro2.x, pro2.y, pro2.z}, {pro3.x, pro3.y, pro3.z} });
				tri.symbol = colour.Char.UnicodeChar;
				tri.colour = colour.Attributes;

				renderTriangles.push_back(tri);	
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
	void coordinate_projection(Vector3D& vertex, Matrix& operation, Vector3D& outVec)
	{
		outVec.x = vertex.x * operation(0, 0) + vertex.y * operation(1, 0) + vertex.z * operation(2, 0) + operation(3, 0);
		outVec.y = vertex.x * operation(0, 1) + vertex.y * operation(1, 1) + vertex.z * operation(2, 1) + operation(3, 1);
		outVec.z = vertex.x * operation(0, 2) + vertex.y * operation(1, 2) + vertex.z * operation(2, 2) + operation(3, 2);

		float leftOver = vertex.x * operation(0, 3) + vertex.y * operation(1, 3) + vertex.z * operation(2, 3) + operation(3, 3);

		// Dived entire matrix by the last value to convert it back to 3D space
		// Also satistfies dividing the terms by z

		if (leftOver != 0.0f)
		{
			outVec.x /= leftOver;
			outVec.y /= leftOver;
			outVec.z /= leftOver;
		}

	}

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
				triangles.push_back(Triangle({
					{vertices[i1 - 1].x, vertices[i1 - 1].y, vertices[i1 - 1].z},
					{vertices[i2 - 1].x, vertices[i2 - 1].y, vertices[i2 - 1].z},
					{vertices[i3 - 1].x, vertices[i3 - 1].y, vertices[i3 - 1].z} }));
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

	float scaling_factor = 1.0f / tanf(theta * 0.5f / 180.0f * 3.14159f); // amount needed to scale coordinates based on the fov
	float aspect_ratio = (float)ScreenHeight() / (float)ScreenWidth();

	float rotate_angle; // perodically changing to give the appearance that the object is rotating

	// Projection matrices
	Vector3D pro1;
	Vector3D pro2;
	Vector3D pro3;

	Vector3D rX1;
	Vector3D rX2;
	Vector3D rX3;

	Vector3D rZ1;
	Vector3D rZ2;
	Vector3D rZ3;

	// Vectors to calculate the normal of triangle faces.
	Vector3D l1;
	Vector3D l2;
	Vector3D normal;

	std::vector<Triangle> renderTriangles;

	
};

int main()
{
	EmazingEngine game;
	if (game.ConstructConsole(264, 250, 4, 4))
		game.Start();
		
	return 0;
}
