#include "Object.h"

#include <fstream>
#include <strstream>
#include <iostream>

Object::Object(const std::string& fname)
{
	m_tris = load_obj_file(fname);
}

std::vector<Triangle> Object::load_obj_file(const std::string& fname)
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
				{ vertices[i1 - 1], vertices[i2 - 1], vertices[i3 - 1] }
			); 
		}
	}

	return triangles;
}
