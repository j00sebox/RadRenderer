#pragma once

#include <nlohmann/json.hpp>

using namespace nlohmann;

struct PrimitiveData
{
    unsigned int position_accessor;
    unsigned int normal_accessor;
    unsigned int texcoord_accessor;
    unsigned int indices_accessor;
    int material_index = -1;
};

class GLTFLoader
{
public:
    GLTFLoader() {}
    explicit GLTFLoader(const char* path);

    void readFile(const char* path);

    [[nodiscard]] std::vector<float> getPositions() const;
    [[nodiscard]] std::vector<float> getNormals() const;
    [[nodiscard]] std::vector<float> getTexCoords() const;
    [[nodiscard]] std::vector<unsigned int> getIndices() const;
    [[nodiscard]] std::vector<int> getMaterialIndices() const;

    [[nodiscard]] std::vector<std::string> getTextures() const;

private:
    void loadBin(const char* file_path);
    void extractFloats(unsigned int accessor_index, std::vector<float>& flts) const;
    void extractIndices(unsigned int accessor_index, std::vector<unsigned int>& indices, unsigned int vertex_offset) const;

    json m_json;
    std::string m_base_dir;

    std::vector<unsigned char> m_data;
    std::vector<PrimitiveData> m_primitives;
};