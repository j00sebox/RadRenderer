#pragma once

#include "../mathz/matrix.hpp"

class Camera
{
public:
	Camera();
	~Camera() {}

	void set_pos(mathz::Vec3&& pos);
	const mathz::Vec3& get_pos() const;

	inline const mathz::Vec3& get_forward() { return m_forward; }
	const mathz::Mat4& get_transform() { return m_transform; }

private:
	mathz::Vec3 m_position;
	mathz::Vec3 m_forward, m_up, m_right;
	mathz::Mat4 m_transform;
};
