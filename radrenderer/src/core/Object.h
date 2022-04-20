#pragma once

#include <string>
#include <vector>

#include "math/Matrix.h"
#include "math/Vector.h"

struct Pixel
{
	Pixel() : r(0), g(0), b(0), a(0) {}

	Pixel(std::uint8_t _r, std::uint8_t _g, std::uint8_t _b, std::uint8_t _a) : r(_r), g(_g), b(_b), a(_a) {}

	std::uint8_t r, g, b, a;
};

struct Triangle
{
	math::Vec3<float> vertices[3];
	math::Vec3<float> normal[3];
	float z[3] = {};
	Pixel colours[3];
};

class Object
{
public:
	Object() = delete;

	Object(const std::string& fname);

	void translate(float x, float y, float z);

	void rotate_x(float rx);
	void rotate_y(float ry);
	void rotate_z(float rz);

	const math::Mat4<float>& get_transform() const { return m_transform; }
	void reset_transform();

	inline std::vector<Triangle>::iterator begin() { return m_tris.begin(); }
	inline std::vector<Triangle>::iterator end() { return m_tris.end(); }
	inline std::vector<Triangle>::const_iterator begin() const { return m_tris.begin(); }
	inline std::vector<Triangle>::const_iterator end() const { return m_tris.end(); }

private:
	// loads vertex and face data from txt file realting to obj file
	void load_obj_file(const std::string& fname);

	std::vector<Triangle> m_tris;
	math::Mat4<float> m_transform;
};
