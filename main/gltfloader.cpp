#include "pch.h"
#include "gltfloader.hpp"

int GetVertexCount(const std::string& type)
{
    if (type == "SCALAR")
        return 1;
    else if (type == "VEC2")
        return 2;
    else if (type == "VEC3")
        return 3;
    else if (type == "VEC4")
        return 4;

    return 0;
}

GLTFLoader::GLTFLoader(const char* path)
{
    readFile(path);
}

void GLTFLoader::readFile(const char* path)
{
    std::string line;
    std::ifstream stream(path);

    std::stringstream ss;

    while (getline(stream, line))
    {
        ss << line << "\n";
    }

    stream.close();

    std::string src = ss.str();

    m_json = json::parse(src);

    std::string uri = m_json["buffers"][0]["uri"];

    std::string p(path);
    m_base_dir = p.substr(0, (p.find_last_of('/') + 1));
    std::string bin_path = m_base_dir + uri;

    loadBin(bin_path.c_str());

    // Iterate all meshes and their primitives
    for (const auto& mesh : m_json["meshes"])
    {
        for (const auto& primitive : mesh["primitives"])
        {
            PrimitiveData prim;

            const auto& attributes = primitive["attributes"];

            prim.position_accessor = attributes["POSITION"];

            if (attributes.contains("NORMAL"))
                prim.normal_accessor = attributes["NORMAL"];

            if (attributes.contains("TEXCOORD_0"))
                prim.texcoord_accessor = attributes["TEXCOORD_0"];

            prim.indices_accessor = primitive["indices"];

            if (primitive.contains("material"))
                prim.material_index = primitive["material"];

            m_primitives.push_back(prim);
        }
    }
}

std::vector<float> GLTFLoader::getPositions() const
{
    std::vector<float> all_positions;

    for (const auto& prim : m_primitives)
    {
        extractFloats(prim.position_accessor, all_positions);
    }

    return all_positions;
}

std::vector<float> GLTFLoader::getNormals() const
{
    std::vector<float> all_normals;

    for (const auto& prim : m_primitives)
    {
        extractFloats(prim.normal_accessor, all_normals);
    }

    return all_normals;
}

std::vector<float> GLTFLoader::getTexCoords() const
{
    std::vector<float> all_texcoords;

    for (const auto& prim : m_primitives)
    {
        extractFloats(prim.texcoord_accessor, all_texcoords);
    }

    return all_texcoords;
}

std::vector<unsigned int> GLTFLoader::getIndices() const
{
    std::vector<unsigned int> all_indices;
    unsigned int vertex_offset = 0;

    for (const auto& prim : m_primitives)
    {
        json accessor = m_json["accessors"][prim.position_accessor];
        unsigned int vertex_count = accessor["count"];

        extractIndices(prim.indices_accessor, all_indices, vertex_offset);

        vertex_offset += vertex_count;
    }

    return all_indices;
}

std::vector<int> GLTFLoader::getMaterialIndices() const
{
    std::vector<int> material_indices;

    for (const auto& primtive : m_primitives)
    {
        json accessor = m_json["accessors"][primtive.indices_accessor];
        unsigned int index_count = accessor["count"];
        unsigned int triangle_count = index_count / 3;

        for (unsigned int i = 0; i < triangle_count; ++i)
        {
            material_indices.push_back(primtive.material_index);
        }
    }

    return material_indices;
}

std::vector<std::string> GLTFLoader::getTextures() const
{
    std::vector<std::string> textures;

    if (!m_json.contains("materials"))
        return textures;

    for (const auto& material : m_json["materials"])
    {
        if (material.contains("pbrMetallicRoughness") &&
            material["pbrMetallicRoughness"].contains("baseColorTexture"))
        {
            int tex_index = material["pbrMetallicRoughness"]["baseColorTexture"]["index"];
            int image_index = m_json["textures"][tex_index]["source"];
            std::string image_uri = m_json["images"][image_index]["uri"];
            textures.push_back(m_base_dir + image_uri);
        }
        else
        {
            // No texture for this material, push empty string
            textures.push_back("");
        }
    }

    return textures;
}

void GLTFLoader::loadBin(const char* file_path)
{
    std::streampos file_size;
    std::ifstream file(file_path, std::ios::binary);

    file.seekg(0, std::ios::end);
    file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    m_data.assign(file_size, 0);
    file.read((char*)&m_data[0], file_size);
}

void GLTFLoader::extractFloats(unsigned int accessor_index, std::vector<float>& flts) const
{
    json accessor = m_json["accessors"][accessor_index];

    unsigned int index = accessor.value("bufferView", 0);
    unsigned int byte_offset = accessor.value("byteOffset", 0);
    unsigned int count = accessor["count"];
    std::string type = accessor["type"];

    json buffer = m_json["bufferViews"][index];
    unsigned int buffer_offset = buffer.value("byteOffset", 0);
    unsigned int buffer_byte_offset = buffer_offset + byte_offset;

    int components = GetVertexCount(type);
    unsigned int element_size = components * 4;
    unsigned int byte_stride = buffer.value("byteStride", element_size);

    for (unsigned int i = 0; i < count; ++i)
    {
        unsigned int offset = buffer_byte_offset + i * byte_stride;
        for (int j = 0; j < components; ++j)
        {
            float val;
            std::memcpy(&val, &m_data[offset + j * 4], sizeof(float));
            flts.push_back(val);
        }
    }
}

void GLTFLoader::extractIndices(unsigned int accessor_index, std::vector<unsigned int>& indices, unsigned int vertex_offset) const
{
    json accessor = m_json["accessors"][accessor_index];

    unsigned int index = accessor.value("bufferView", 0);
    unsigned int byte_offset = accessor.value("byteOffset", 0);
    unsigned int count = accessor["count"];
    unsigned int component_type = accessor["componentType"];

    json buffer = m_json["bufferViews"][index];
    unsigned int buffer_byte_offset = buffer.value("byteOffset", 0) + byte_offset;

    switch (component_type)
    {
    case 5125: // UNSIGNED_INT
    {
        unsigned int length = count * 4;

        for (unsigned int i = buffer_byte_offset; i < (buffer_byte_offset + length);)
        {
            unsigned char byte[] = { m_data[i++], m_data[i++], m_data[i++], m_data[i++] };
            unsigned int val;

            std::memcpy(&val, byte, sizeof(unsigned int));

            indices.push_back(val + vertex_offset);
        }
        break;
    }

    case 5123: // UNSIGNED_SHORT
    {
        unsigned int length = count * 2;

        for (unsigned int i = buffer_byte_offset; i < (buffer_byte_offset + length);)
        {
            unsigned char byte[] = { m_data[i++], m_data[i++] };
            unsigned short val;

            std::memcpy(&val, byte, sizeof(unsigned short));

            indices.push_back(val + vertex_offset);
        }
        break;
    }
    }
}
