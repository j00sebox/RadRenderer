#pragma once

#include "texture.hpp"

#include "../mathz/matrix.hpp"
#include "../mathz/quaternion.hpp"
#include "../mathz/vector.hpp"

#include <vector>

// SoA layout for cache-friendly access
// Each triangle i has:
//   vertices at index i*3, i*3+1, i*3+2
//   normals at index i*3, i*3+1, i*3+2
//   uvs at index i*3, i*3+1, i*3+2
//   z at index i*3, i*3+1, i*3+2
//   material at index i
struct MeshData
{
  std::vector<mathz::Vec3> vertices;  // 3 per triangle
  std::vector<mathz::Vec3> normals;   // 3 per triangle
  std::vector<mathz::Vec2<float>> uvs; // 3 per triangle
  std::vector<float> z;               // 3 per triangle (filled during rendering)
  std::vector<int> materials;         // 1 per triangle

  void reserve(size_t triangle_count)
  {
    vertices.reserve(triangle_count * 3);
    normals.reserve(triangle_count * 3);
    uvs.reserve(triangle_count * 3);
    z.reserve(triangle_count * 3);
    materials.reserve(triangle_count);
  }

  void clear()
  {
    vertices.clear();
    normals.clear();
    uvs.clear();
    z.clear();
    materials.clear();
  }

  size_t triangleCount() const { return materials.size(); }
};

class Model
{
public:
  Model() = delete;
  Model(const char* file_name);

  void setPosition(float x, float y, float z);
  void setRotation(const mathz::Quaternion& rotation);
  void setScale(float scale);

  const mathz::Mat4& getTransform() const { return m_transform; }
  void resetTransform();

  const Texture& getTexture(int index) const { return m_textures[index]; }
  const std::vector<Texture>& getTextures() const { return m_textures; }

  const MeshData& getMeshData() const { return m_mesh; }
  inline std::size_t size() const { return m_mesh.triangleCount(); }

  void loadGLTF(const char* file_name);

private:
  void updateTransform();
  void loadOBJ(const char* file_name);

  MeshData m_mesh;
  mathz::Mat4 m_translation, m_rotation, m_scale;
  mathz::Mat4 m_transform;
  std::vector<Texture> m_textures;
};
