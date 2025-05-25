#include "model.hpp"
#include "pch.h"

Model::Model(const char* file_name)
{
  LoadOBJFile(std::move(file_name));
}

Model::Model(std::vector<Triangle>&& triangles)
    : m_triangles(triangles)
{
}

void Model::SetPosition(float x, float y, float z)
{
  m_translation = mathz::Mat4({1, 0, 0, x,
                               0, 1, 0, y,
                               0, 0, 1, z,
                               0, 0, 0, 1});

  UpdateTransform();
}

void Model::SetRotation(const mathz::Quaternion& rotation)
{
  m_rotation = rotation.ToMatrix();

  UpdateTransform();
}

void Model::SetScale(float scale)
{
  m_scale = mathz::Mat4({scale, 0, 0, 0,
                         0, scale, 0, 0,
                         0, 0, scale, 0,
                         0, 0, 0, 1});

  UpdateTransform();
}

void Model::UpdateTransform()
{
  m_transform = m_scale * m_rotation * m_translation;
}

void Model::ResetTransform()
{
  m_transform.clear();
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
      m_triangles.push_back(
          {vertices[i1 - 1], vertices[i2 - 1], vertices[i3 - 1]});
    }
  }
}
