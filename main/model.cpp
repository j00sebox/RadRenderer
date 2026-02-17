#include "pch.h"

#include "model.hpp"
#include "gltfloader.hpp"

Model::Model(const char* file_name)
{
    std::string path(file_name);
    std::string ext = path.substr(path.find_last_of('.') + 1);

    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "obj")
    {
        loadOBJ(file_name);
    }
    else if (ext == "gltf")
    {
        loadGLTF(file_name);
    }
    else
    {
        std::cerr << "Unknown file format: " << ext << std::endl;
    }
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
    else if (type == "f")
    {
      int index[3];
      for (int i = 0; i < 3; ++i)
      {
        std::string token;
        ss >> token;
        std::istringstream tokenStream(token);
        std::string indexStr;
        std::getline(tokenStream, indexStr, '/');
        index[i] = std::stoi(indexStr) - 1;
      }

      m_mesh.vertices.push_back(vertices[index[0]]);
      m_mesh.vertices.push_back(vertices[index[1]]);
      m_mesh.vertices.push_back(vertices[index[2]]);

      m_mesh.normals.push_back({});
      m_mesh.normals.push_back({});
      m_mesh.normals.push_back({});

      m_mesh.uvs.push_back({});
      m_mesh.uvs.push_back({});
      m_mesh.uvs.push_back({});

      m_mesh.z.push_back(0.f);
      m_mesh.z.push_back(0.f);
      m_mesh.z.push_back(0.f);

      m_mesh.materials.push_back(0);
    }
  }

  // Add default texture for OBJ
  m_textures.emplace_back();
}

void Model::loadGLTF(const char* file_name)
{
    GLTFLoader loader;
    loader.readFile(file_name);

    std::vector<float> vertices = loader.getPositions();
    std::vector<unsigned int> indices = loader.getIndices();
    std::vector<float> uvs = loader.getTexCoords();
    std::vector<int> material_indices = loader.getMaterialIndices();

    size_t triangle_count = indices.size() / 3;
    m_mesh.reserve(triangle_count);

    for (size_t i = 0; i < indices.size(); i += 3)
    {
        int idx0 = indices[i];
        int idx1 = indices[i + 1];
        int idx2 = indices[i + 2];

        m_mesh.vertices.push_back({
            vertices[idx0 * 3],
            vertices[idx0 * 3 + 1],
            vertices[idx0 * 3 + 2]
        });
        m_mesh.vertices.push_back({
            vertices[idx1 * 3],
            vertices[idx1 * 3 + 1],
            vertices[idx1 * 3 + 2]
        });
        m_mesh.vertices.push_back({
            vertices[idx2 * 3],
            vertices[idx2 * 3 + 1],
            vertices[idx2 * 3 + 2]
        });

        m_mesh.uvs.push_back({
            uvs[idx0 * 2],
            uvs[idx0 * 2 + 1]
        });
        m_mesh.uvs.push_back({
            uvs[idx1 * 2],
            uvs[idx1 * 2 + 1]
        });
        m_mesh.uvs.push_back({
            uvs[idx2 * 2],
            uvs[idx2 * 2 + 1]
        });

        // Normals computed during rendering
        m_mesh.normals.push_back({});
        m_mesh.normals.push_back({});
        m_mesh.normals.push_back({});

        // Z filled during rendering
        m_mesh.z.push_back(0.f);
        m_mesh.z.push_back(0.f);
        m_mesh.z.push_back(0.f);

        m_mesh.materials.push_back(material_indices[i / 3]);
    }

    std::vector<std::string> texture_paths = loader.getTextures();

    for (const std::string& texture_path : texture_paths)
    {
        if (!texture_path.empty())
            m_textures.emplace_back(texture_path.c_str());
        else
            m_textures.emplace_back();
    }

    // Add default pink texture at the end for triangles with no material
    int default_material_index = static_cast<int>(m_textures.size());
    m_textures.emplace_back();

    // Fix up any invalid material indices
    for (int& mat : m_mesh.materials)
    {
        if (mat < 0 || mat >= default_material_index)
            mat = default_material_index;
    }
}
