#include "pch.h"
#include "Camera.h"

Camera::Camera()
	: m_position(math::Vec3<float>()),
	m_rotation_x(math::Mat4<float>()), m_rotation_y(math::Mat4<float>()), m_rotation_z(math::Mat4<float>())
{
}

void Camera::set_pos(math::Vec3<float> pos)
{
	m_position = pos;
}

const math::Vec3<float>& Camera::get_pos() const
{
	return m_position;
}

void Camera::set_rot_x(float rx)
{
}

void Camera::set_rot_y(float ry)
{
}

void Camera::set_rot_z(float rz)
{
}

const math::Mat4<float>& Camera::get_rot_x() const
{
	return m_rotation_x;
}

const math::Mat4<float>& Camera::get_rot_y() const
{
	return m_rotation_y;
}

const math::Mat4<float>& Camera::get_rot_z() const
{
	return m_rotation_z;
}
