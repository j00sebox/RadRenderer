#pragma once

#include <vector>

struct Colour
{
    float r, g, b, a;
};

class Texture
{
public:
    Texture() {}
    Texture(const char* image_path);
    ~Texture();

    enum class Filter
    {
        Nearest,
        Bilinear
    };

    Colour sample(float u, float v, Filter filter = Filter::Nearest) const;

private:
    int m_width, m_height;
    int m_channels;
    std::vector<unsigned char> m_data; // Raw pixel data

    Colour getPixel(int x, int y) const;
    Colour sampleNearest(float u, float v) const;
};
