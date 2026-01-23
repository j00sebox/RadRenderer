#include "pch.h"
#include "texture.hpp"

#include <stb_image.h>

Texture::Texture()
    : m_width(1), m_height(1), m_channels(4), m_data{255, 255, 255, 255}
{
}

Texture::Texture(const char* image_path)
{
    unsigned char* rawData = stbi_load(image_path, &m_width, &m_height, &m_channels, 4);
    if (!rawData) 
    {
        throw std::runtime_error("Failed to load texture");
    }

    m_data.assign(rawData, rawData + (m_width * m_height * 4));
    stbi_image_free(rawData);
}

Texture::~Texture()
{

}

Colour Texture::sample(float u, float v, Filter filter) const
{
    switch (filter) 
    {
    case Filter::Nearest:  return sampleNearest(u, v);
    default: return sampleNearest(u, v);
    }
}

Colour Texture::getPixel(int x, int y) const
{
    x = std::clamp(x, 0, m_width - 1);
    y = std::clamp(y, 0, m_height - 1);
    int index = (y * m_width + x) * 4;
    return {
        m_data[index] / 255.0f,
        m_data[index + 1] / 255.0f,
        m_data[index + 2] / 255.0f,
        m_data[index + 3] / 255.0f
    };
}

Colour Texture::sampleNearest(float u, float v) const
{
    int x = static_cast<int>(u * (m_width - 1));
    int y = static_cast<int>(v * (m_height - 1));
    return getPixel(x, y);
}

