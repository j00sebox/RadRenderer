#pragma once

#include <string>
#include <vector>

#include "../mathz/matrix.hpp"
#include "../mathz/vector.hpp"
#include "../mathz/quaternion.hpp"

struct Pixel
{
	Pixel() : r(0), g(0), b(0), a(0) {}

	Pixel(std::uint8_t _r, std::uint8_t _g, std::uint8_t _b, std::uint8_t _a) : r(_r), g(_g), b(_b), a(_a) {}

	std::uint8_t r, g, b, a;
};

struct Triangle
{
	mathz::Vec3 vertices[3];
	mathz::Vec3 normal[3];
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

	const mathz::Mat4& get_transform() const { return m_transform; }
	const mathz::Quaternion& get_quaternion() const { return m_qrotation; }
	void reset_transform();

	void operator= (const mathz::Quaternion& quat);

	inline std::vector<Triangle>::iterator begin() { return m_tris.begin(); }
	inline std::vector<Triangle>::iterator end() { return m_tris.end(); }
	inline std::vector<Triangle>::const_iterator begin() const { return m_tris.begin(); }
	inline std::vector<Triangle>::const_iterator end() const { return m_tris.end(); }

private:
	// loads vertex and face data from obj file
	void load_obj_file(const std::string& fname);

	std::vector<Triangle> m_tris;
	mathz::Mat4 m_transform;
	mathz::Quaternion m_qrotation;
};
