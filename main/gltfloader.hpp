#pragma once

#include <nlohmann/json.hpp>

using namespace nlohmann;

class GLTFLoader
{
public:
    GLTFLoader() {}
    explicit GLTFLoader(const char* path);

    void ReadFile(const char* path);

    [[nodiscard]] std::vector<float> GetPositions() const;
    [[nodiscard]] std::vector<float> GetNormals() const;
    [[nodiscard]] std::vector<float> GetTexCoords() const;
    [[nodiscard]] std::vector<unsigned int> GetIndices() const;

    [[nodiscard]] std::vector<std::string> GetTextures() const;
    [[nodiscard]] std::string GetBaseColourTexture() const;
    [[nodiscard]] std::string GetSpecularTexture() const;
    [[nodiscard]] std::string GetNormalTexture() const;
    [[nodiscard]] std::string GetOcclusionTexture() const;

private:
    void LoadBin(const char* file_path);
    void ExtractFloats(const json& accessor, std::vector<float>& flts) const;

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