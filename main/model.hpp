#pragma once

#include "../mathz/matrix.hpp"
#include "../mathz/quaternion.hpp"
#include "../mathz/vector.hpp"

#include <vector>

struct Triangle
{
  mathz::Vec3 vertices[3];
  mathz::Vec3 normal[3];
  float z[3] = {};

  mathz::Vec3 Center()
  {
    return (vertices[0] + vertices[1] + vertices[2]) * 0.3333f;
  }
};

class Model
{
public:
  Model() = delete;
  Model(const char* file_name);
  Model(const std::vector<Triangle>& triangles);

  void SetPosition(float x, float y, float z);
  void SetRotation(const mathz::Quaternion& rotation);
  void SetScale(float scale);

  const mathz::Mat4& GetTransform() const { return m_transform; }
  void ResetTransform();

  inline std::vector<Triangle>::iterator begin() { return m_triangles.begin(); }
  inline std::vector<Triangle>::iterator end() { return m_triangles.end(); }
  inline std::vector<Triangle>::const_iterator begin() const { return m_triangles.begin(); }
  inline std::vector<Triangle>::const_iterator end() const { return m_triangles.end(); }

  void LoadGLTFFile(const char* file_name);

private:
  void UpdateTransform();

  // Loads vertex and face data from obj file
  void LoadOBJFile(const char* file_name);
  

  std::vector<Triangle> m_triangles;
  mathz::Mat4 m_translation, m_rotation, m_scale;
  mathz::Mat4 m_transform;
};
