#include "pch.h"
#include "Object.h"

#include "mathz/Quaternion.h"

Object::Object(const std::string& fname)
	: m_qrotation(1.f, 0.f, 0.f, 0.f)
{
	load_obj_file(fname);
}

void Object::translate(float x, float y, float z)
{
	m_transform = 

		mathz::Mat4({
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			x, y, z, 1
		});
}

void Object::rotate_x(float rx)
{
	m_transform = m_transform *

		mathz::Mat4({
			1, 0,			0,			0,
			0, cosf(rx),	sinf(rx),	0,
			0, -sinf(rx),	cosf(rx),	0,
			0, 0,			0,			1
		});
}

void Object::rotate_y(float ry)
{
	m_transform = m_transform *

		mathz::Mat4({
			cosf(ry),	0, sinf(ry),	0,
			0,			1, 0,			0,
			-sinf(ry),	0, cosf(ry),	0,
			0,			0, 0,			1
		});
}

void Object::rotate_z(float rz)
{
	m_transform = m_transform *

		mathz::Mat4({
			cosf(rz),	sinf(rz),	0, 0,
			-sinf(rz),	cosf(rz),	0, 0,
			0,			0,			1, 0,
			0,			0,			0, 1
		});
}

void Object::reset_transform()
{
	m_transform.clear();
}

void Object::operator=(const mathz::Quaternion& quat)
{
	m_qrotation = quat;
}

void Object::load_obj_file(const std::string& fname)
{
	std::ifstream readFile;
	readFile.open(fname, std::ifstream::in);

	if (!readFile.is_open())
	{
		std::cout << "Cannot open file!\n";
	}

	mathz::Vec3 vertex;
	std::vector<mathz::Vec3> vertices;

	std::string line;

	char startingChar; // Stores the starting character of the line

	int i1, i2, i3; // indexes of the vertices

	// iterate through all lines in file
	while (std::getline(readFile, line))
	{
		std::stringstream st;
		st << line;

		// indicates vertex data
		if (line[0] == 'v')
		{
			st >> startingChar >> vertex.x >> vertex.y >> vertex.z;
			vertices.emplace_back(vertex);
		}
		// indicates triangle face data
		else if (line[0] == 'f')
		{
			st >> startingChar >> i1 >> i2 >> i3;
			m_tris.push_back(
				{ vertices[i1 - 1], vertices[i2 - 1], vertices[i3 - 1] }
			);
		}
	}
}
