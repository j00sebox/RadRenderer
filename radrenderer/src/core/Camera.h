#pragma once

#include "math/Matrix.h"

class Camera
{
public:
	Camera();
	~Camera() {}

	void set_pos(math::Vec3<float> pos);
	const math::Vec3<float>& get_pos() const;

	void set_rot_x(float rx);
	void set_rot_y(float ry);
	void set_rot_z(float rz);

	const math::Mat4<float>& get_rot_x() const;
	const math::Mat4<float>& get_rot_y() const;
	const math::Mat4<float>& get_rot_z() const;

private:
	math::Vec3<float> m_position;
	math::Mat4<float> m_rotation_x, m_rotation_y, m_rotation_z;
};