#include "model.hpp"
#include "pch.h"

Model::Model(const char* file_name)
    : m_qrotation(1.f, 0.f, 0.f, 0.f)
{
  LoadOBJFile(std::move(file_name));
}

Model::Model(std::vector<Triangle>&& triangles)
    : m_qrotation(1.f, 0.f, 0.f, 0.f),
      m_tris(triangles)
{
}

void Model::Translate(float x, float y, float z)
{
  m_transform = mathz::Mat4({1, 0, 0, 0,
                             0, 1, 0, 0,
                             0, 0, 1, 0,
                             x, y, z, 1});
}

void Model::RotateX(float rx)
{
  m_transform = m_transform * mathz::Mat4({1, 0, 0, 0,
                                           0, cosf(rx), sinf(rx), 0,
                                           0, -sinf(rx), cosf(rx), 0,
                                           0, 0, 0, 1});
}

void Model::RotateY(float ry)
{
  m_transform = m_transform * mathz::Mat4({cosf(ry), 0, sinf(ry), 0,
                                           0, 1, 0, 0,
                                           -sinf(ry), 0, cosf(ry), 0,
                                           0, 0, 0, 1});
}

void Model::RotateZ(float rz)
{
  m_transform = m_transform * mathz::Mat4({cosf(rz), sinf(rz), 0, 0,
                                           -sinf(rz), cosf(rz), 0, 0,
                                           0, 0, 1, 0,
                                           0, 0, 0, 1});
}

void Model::ResetTransform()
{
  m_transform.clear();
}

void Model::operator=(const mathz::Quaternion& quat)
{
  m_qrotation = quat;
}

void Model::LoadOBJFile(const char* file_name)
{
  std::ifstream readFile;
  readFile.open(file_name, std::ifstream::in);

  if (!readFile.is_open())
  {
    std::cout << "Cannot open file!\n";
  }

  mathz::Vec3 vertex;
  std::vector<mathz::Vec3> vertices;

  std::string line;

  char startingChar; // Stores the starting character of the line

  int i1, i2, i3; // Indexes of the vertices

  // Iterate through all lines in file
  while (std::getline(readFile, line))
  {
    std::stringstream st;
    st << line;

    // Indicates vertex data
    if (line[0] == 'v')
    {
      st >> startingChar >> vertex.x >> vertex.y >> vertex.z;
      vertices.emplace_back(vertex);
    }
    // Indicates triangle face data
    else if (line[0] == 'f')
    {
      st >> startingChar >> i1 >> i2 >> i3;
      m_tris.push_back(
          {vertices[i1 - 1], vertices[i2 - 1], vertices[i3 - 1]});
    }
  }
}
