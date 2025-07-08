#pragma once

#include <nlohmann/json.hpp>

using namespace nlohmann;

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

    [[nodiscard]] std::vector<std::string> getTextures() const;
    [[nodiscard]] std::string getBaseColourTexture() const;
    [[nodiscard]] std::string getSpecularTexture() const;
    [[nodiscard]] std::string getNormalTexture() const;
    [[nodiscard]] std::string getOcclusionTexture() const;

private:
    void loadBin(const char* file_path);
    void extractFloats(const json& accessor, std::vector<float>& flts) const;

    json m_json;
    std::string m_base_dir;

    std::vector<unsigned char> m_data;

    unsigned int m_position_ind;
    unsigned int m_normal_ind;
    unsigned int m_tex_coord_ind;
    unsigned int m_indices_ind;

    unsigned int m_bc_tex_ind;
    int m_spec_tex_ind = -1;
    int m_norm_tex_ind = -1;
    int m_occ_tex_ind = -1;
};