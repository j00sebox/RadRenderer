#include <iostream>
#include <algorithm>

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
	
	Triangle(std::vector< std::vector<float > > v) : vertices(3, 3)
	{
		vertices.Assign(v);
	}

	Matrix vertices;

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

class EmazingEngine : public olcConsoleGameEngine
{
public:
	bool OnUserCreate() override
	{
#ifdef CUBE_DEMO
		object = cube_demo();		
#endif  CUBE_DEMO

		return true;

	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear screen to redraw
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

		rotate_angle += 1.0f * fElapsedTime;

		// Projection matrices
		Matrix pro1(1, 3);
		Matrix pro2(1, 3);
		Matrix pro3(1, 3);

		Matrix rX1(1, 3);
		Matrix rX2(1, 3);
		Matrix rX3(1, 3);

		Matrix rZ1(1, 3);
		Matrix rZ2(1, 3);
		Matrix rZ3(1, 3);

		// Vectors to calculate the normal of triangle faces.
		Vector3D l1;
		Vector3D l2;
		Vector3D normal;

		// Position of camera aka user's view
		Vector3D cam;

		// Direction that the light would be pointing in game
		Vector3D lighting;
		lighting.Assign({ {0.0f, 0.0f, -1.0f} });

		lighting.normalize();

		for (auto t : object)
		{
			// Rotate around z-axis
			rZ1 = z_axis_rotation(t.vertices(0, 0), t.vertices(0, 1), t.vertices(0, 2));
			rZ2 = z_axis_rotation(t.vertices(1, 0), t.vertices(1, 1), t.vertices(1, 2));
			rZ3 = z_axis_rotation(t.vertices(2, 0), t.vertices(2, 1), t.vertices(2, 2));

			// Rotate around x-axis
			rX1 = x_axis_rotation(rZ1(0, 0), rZ1(0, 1), rZ1(0, 2));
			rX2 = x_axis_rotation(rZ2(0, 0), rZ2(0, 1), rZ2(0, 2));
			rX3 = x_axis_rotation(rZ3(0, 0), rZ3(0, 1), rZ3(0, 2));

			// move object back so it is in view of the camera
			rX1.value[0][2] += 3.0f;
			rX2.value[0][2] += 3.0f;
			rX3.value[0][2] += 3.0f;

			// Construct line 1 of the triangle
			l1.x = rX2(0, 0) - rX1(0, 0);
			l1.y = rX2(0, 1) - rX1(0, 1);
			l1.z = rX2(0, 2) - rX1(0, 2);

			// Contstruct line 2 of the triangle
			l2.x = rX3(0, 0) - rX1(0, 0);
			l2.y = rX3(0, 1) - rX1(0, 1);
			l2.z = rX3(0, 2) - rX1(0, 2);

			// calculate normal of triangle face
			normal = l1.cross(l2);

			normal.normalize();

			// Calculate the angle betwwen the normal and the camera projection to determine if the triangle is visible
			if ( (normal.x * rX1(0, 0) - cam.x
				+ normal.y * rX1(0, 1) - cam.y
				+ normal.z * rX1(0, 2) - cam.z ) < 0 )
			{
				// Dot product is used to determine how direct the light is hitting the traingle to determine what shade it should be
				float dotProd = (normal * lighting.transpose())(0, 0);

				// The larger dot product in this case means the more lit up the triangle face will be 
				CHAR_INFO colour = GetColour(dotProd);
				
				// Project all the coordinates to 2D space
				pro1 = coordinate_projection(rX1(0, 0), rX1(0, 1), rX1(0, 2));
				pro2 = coordinate_projection(rX2(0, 0), rX2(0, 1), rX2(0, 2));
				pro3 = coordinate_projection(rX3(0, 0), rX3(0, 1), rX3(0, 2));

				// Center the points and change the scale
				pro1.value[0][0] += 1.0f;
				pro1.value[0][1] += 1.0f;

				pro2.value[0][0] += 1.0f;
				pro2.value[0][1] += 1.0f;

				pro3.value[0][0] += 1.0f;
				pro3.value[0][1] += 1.0f;

				pro1.value[0][0] *= 0.5f * (float)ScreenWidth();
				pro1.value[0][1] *= 0.5f * (float)ScreenHeight();

				pro2.value[0][0] *= 0.5f * (float)ScreenWidth();
				pro2.value[0][1] *= 0.5f * (float)ScreenHeight();

				pro3.value[0][0] *= 0.5f * (float)ScreenWidth();
				pro3.value[0][1] *= 0.5f * (float)ScreenHeight();

				Triangle tri({ {pro1(0, 0), pro1(0, 1), pro1(0, 2)}, {pro2(0, 0), pro2(0, 1), pro2(0, 2)}, {pro3(0, 0), pro3(0, 1), pro3(0, 2)} });
				tri.symbol = colour.Char.UnicodeChar;
				tri.colour = colour.Attributes;

				renderTriangles.push_back(tri);

				
			}
	
		}

		std::sort(renderTriangles.begin(), renderTriangles.end(), [](Triangle& tri1, Triangle& tri2) {
				float avg_z1 = (tri1.vertices(0, 2) + tri1.vertices(1, 2) + tri1.vertices(2, 2)) / 3.0f;
				float avg_z2 = (tri2.vertices(0, 2) + tri2.vertices(1, 2) + tri2.vertices(2, 2)) / 3.0f;

				if (avg_z1 > avg_z2)
					return true;
				else
					return false;
			});

		for (auto& t : renderTriangles) 
		{
			FillTriangle(t.vertices(0, 0), t.vertices(0, 1), t.vertices(1, 0), t.vertices(1, 1), t.vertices(2, 0), t.vertices(2, 1), t.symbol, t.colour);
		}
			
		

		return true;
	}

	// Map 3D coordinates to 2D space
	Matrix coordinate_projection(float x, float y, float z)
	{
		// Put coordinates in vector format
		Matrix coordinate_vec(1, 4);
		coordinate_vec.Assign({ { x, y, z, 1.0f } }); // 1 is added so that -z_near*q can be subtracted from z*q

		Matrix projection_matrix(4, 4);
		projection_matrix.Assign({ { aspect_ratio * scaling_factor, 0.0f, 0.0f, 0.0f }, { 0.0f, scaling_factor, 0.0f, 0.0f }, { 0.0f, 0.0f, q, 1.0f } , { 0.0f, 0.0f, -z_near * q, 0.0f } });

		Matrix res = coordinate_vec * projection_matrix;

		// Dived entire matrix by the last value to convert it back to 3D space
		// Also satistfies dividing the terms by z
		res = res / res.value[0][3];

		// Change matrix to 1X3
		res.clip();

		return res;
	}

	Matrix x_axis_rotation(float x, float y, float z)
	{
		// Put coordinates in vector format
		Matrix coordinate_vec(1, 4);
		coordinate_vec.Assign({ { x, y, z, 1.0f } });

		Matrix x_rotation(4, 4);
		x_rotation.Assign({ {1, 0, 0, 0}, {0, cosf(rotate_angle * 0.5f), sinf(rotate_angle * 0.5f), 0}, {0, -sinf(rotate_angle * 0.5f), cosf(rotate_angle * 0.5f), 0}, {0, 0, 0, 1} });

		Matrix res = coordinate_vec * x_rotation;

		// Dived entire matrix by the last value to convert it back to 3D space
		// Also satistfies dividing the terms by z
		res = res / res.value[0][3];

		// Change matrix to 1X3
		res.clip();

		return res;
	}

	Matrix z_axis_rotation(float x, float y, float z)
	{
		// Put coordinates in vector format
		Matrix coordinate_vec(1, 4);
		coordinate_vec.Assign({ { x, y, z, 1.0f } });

		Matrix z_rotation(4, 4);
		z_rotation.Assign({ {cosf(rotate_angle), sinf(rotate_angle), 0, 0}, {-sinf(rotate_angle), cosf(rotate_angle), 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1} });

		Matrix res = coordinate_vec * z_rotation;

		// Dived entire matrix by the last value to convert it back to 3D space
		// Also satistfies dividing the terms by z
		res = res / res.value[0][3];

		// Change matrix to 1X3
		res.clip();

		return res;
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

	std::vector<Triangle> renderTriangles;

	
};

int main()
{
	EmazingEngine game;
	if (game.ConstructConsole(264, 250, 4, 4))
		game.Start();

	return 0;
}
