#include "pch.h"
#include "Camera.h"

Camera::Camera()
{
	m_forward = { 0.f, 0.f, 1.f };
	m_up = { 0.f, 1.f, 0.f };
	m_right = { 1.f, 0.f, 0.f };
}

void Camera::set_pos(math::Vec3&& pos)
{
    m_position = pos;
    
	m_forward = m_forward - pos;
    
	// Calculate the up vector in relation to the new camera direction
	math::Vec3 tmp(0.f, 1.f, 0.f);
	m_right = tmp.cross(m_forward);
    m_right.normalize();

	m_up = m_forward.cross(m_right);
    m_up.normalize();

	// Set up camera direction matrix
	m_transform(0, 0) = m_right.x;			m_transform(0, 1) = m_right.y;			m_transform(0, 2) = m_right.z;			m_transform(0, 3) = 0.0f;
	m_transform(1, 0) = m_up.x;				m_transform(1, 1) = m_up.y;				m_transform(1, 2) = m_up.z;				m_transform(1, 3) = 0.0f;
	m_transform(2, 0) = m_forward.x;		m_transform(2, 1) = m_forward.y;		m_transform(2, 2) = m_forward.z;		m_transform(2, 3) = 0.0f;
	m_transform(3, 0) = m_position.x;		m_transform(3, 1) = m_position.y;		m_transform(3, 2) = m_position.z;		m_transform(3, 3) = 1.0f;
    
    m_forward.normalize();
}

const math::Vec3& Camera::get_pos() const
{
	return m_position;
}
