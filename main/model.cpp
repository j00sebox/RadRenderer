#include "pch.h"

#include "model.hpp"
#include "gltfloader.hpp"

Model::Model(const char* file_name)
{
   // LoadOBJFile(file_name);
   loadGLTF(file_name);
}

Model::Model(const std::vector<Triangle>& triangles)
    : m_triangles(triangles)
{
}

void Model::setPosition(float x, float y, float z)
{
  m_translation = mathz::Mat4({1, 0, 0, x,
                               0, 1, 0, y,
                               0, 0, 1, z,
                               0, 0, 0, 1});

  updateTransform();
}

void Model::setRotation(const mathz::Quaternion& rotation)
{
  m_rotation = rotation.ToMatrix();

  updateTransform();
}

void Model::setScale(float scale)
{
  m_scale = mathz::Mat4({scale, 0, 0, 0,
                         0, scale, 0, 0,
                         0, 0, scale, 0,
                         0, 0, 0, 1});

  updateTransform();
}

void Model::updateTransform()
{
    m_transform = m_translation * m_rotation * m_scale;
}

void Model::resetTransform()
{
  m_transform.clear();
}

Colour Model::sampleTexture(int u, int v, int index)
{
    return m_textures[index].sample(u, v);
}

void Model::loadOBJ(const char* file_name)
{
  std::ifstream file(file_name);
  std::vector<mathz::Vec3> vertices;

  if (!file.is_open())
  {
    std::cerr << "Failed to open " << file_name << std::endl;
    return;
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

void Model::loadGLTF(const char* file_name)
{
    GLTFLoader loader;
    loader.readFile(file_name);

    std::vector<float> vertices = loader.getPositions();
    std::vector<unsigned int> indices = loader.getIndices();
    std::vector<float> uvs = loader.getTexCoords();

    for (int i = 0; i < indices.size(); i += 3)
    {
        int index1 = indices[i];
        int index2 = indices[i + 1];
        int index3 = indices[i + 2];

        mathz::Vec3 vertex1 = {
            vertices[index1 * 3],
            vertices[index1 * 3 + 1],
            vertices[index1 * 3 + 2]
        };

        mathz::Vec3 vertex2 = {
            vertices[index2 * 3],
            vertices[index2 * 3 + 1],
            vertices[index2 * 3 + 2]
        };

        mathz::Vec3 vertex3 = {
            vertices[index3 * 3],
            vertices[index3 * 3 + 1],
            vertices[index3 * 3 + 2]
        };

        mathz::Vec2<float> uv1 = {
            uvs[index1 * 2],
            uvs[index1 * 2 + 1]
        };

        mathz::Vec2<float> uv2 = {
            uvs[index2 * 2],
            uvs[index2 * 2 + 1]
        };

        mathz::Vec2<float> uv3 = {
            uvs[index3 * 2],
            uvs[index3 * 2 + 1]
        };

        m_triangles.push_back({
            .vertices = { vertex1, vertex2, vertex3 },
            .uv = { uv1, uv2, uv3 }
        });
    }

    std::vector<std::string> texture_paths = loader.getTextures();

    int i = 0;
    for (std::string texture_path : texture_paths)
    {
        m_textures[i] = Texture(texture_path.c_str());
        ++i;
    }
}
