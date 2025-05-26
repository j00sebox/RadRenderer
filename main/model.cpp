#include "model.hpp"
#include "pch.h"

Model::Model(const char* file_name)
{
  LoadOBJFile(std::move(file_name));
}

Model::Model(const std::vector<Triangle>& triangles)
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
  std::ifstream file(file_name);
  std::vector<mathz::Vec3> vertices;

  if (!file.is_open())
  {
    std::cerr << "Failed to open " << file_name << std::endl;
  }

  std::string line;
  while (std::getline(file, line))
  {
    std::istringstream ss(line);
    std::string type;
    ss >> type;

    if (type == "v")
    {
      mathz::Vec3 v;
      ss >> v.x >> v.y >> v.z;
      vertices.push_back(v);
    }
    // Gives the indices of the vertices that make up each triangle
    // Denoted by f for face
    else if (type == "f")
    {
      int index[3];
      for (int i = 0; i < 3; ++i)
      {
        std::string token;
        ss >> token;
        std::istringstream tokenStream(token);
        std::string indexStr;
        std::getline(tokenStream, indexStr, '/'); // Only read vertex index
        index[i] = std::stoi(indexStr) - 1;       // OBJ indices start at 1
      }
      m_triangles.push_back({vertices[index[0]], vertices[index[1]], vertices[index[2]]});
    }
  }
}
