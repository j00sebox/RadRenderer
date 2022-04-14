#pragma once

#include "math/Matrix.h"

class Camera
{
public:
	Camera();
	~Camera() {}

	void set_pos(const math::Vec3<float>& pos);
	void set_pos(math::Vec3<float>&& pos);
	const math::Vec3<float>& get_pos() const;

	inline const math::Vec3<float>& get_forward() { return m_forward; }
	const math::Mat4<float>& get_transform() { return m_transform; }

private:
	math::Vec3<float> m_position;
	math::Vec3<float> m_forward, m_up, m_right;
	math::Mat4<float> m_transform;
};
