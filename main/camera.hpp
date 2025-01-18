#pragma once

#include "../mathz/matrix.hpp"

class Camera
{
public:
	Camera();
	~Camera() {}

	void SetPosition(mathz::Vec3&& pos);
	const mathz::Vec3& GetPosition() const;

	inline const mathz::Vec3& GetForward() { return m_forward; }
	const mathz::Mat4& GetTransform() { return m_transform; }

private:
	mathz::Vec3 m_position;
	mathz::Vec3 m_forward, m_up, m_right;
	mathz::Mat4 m_transform;
};
