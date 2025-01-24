#include "camera.hpp"
#include "pch.h"

#define DEG_TO_RAD(x) ((x / 180.0f) * 3.14159f)

Camera::Camera(CameraSettings&& camera_settings)
    : m_near(camera_settings.near),
      m_far(camera_settings.far),
      m_fov(camera_settings.fov)
{
  m_forward = {0.f, 0.f, 1.f};
  m_up = {0.f, 1.f, 0.f};
  m_right = {1.f, 0.f, 0.f};

  float fovRAD = DEG_TO_RAD(m_fov);
  float scaling_factor = 1.0f / tanf(m_fov * 0.5f);
  float aspect_ratio = (float)camera_settings.height / (float)camera_settings.width;

  float q = m_far / (m_far - m_near);

  // Create projection matrix
  m_perspective.set(
      aspect_ratio * scaling_factor, 0.f, 0.f, 0.f,
      0.f, scaling_factor, 0.f, 0.f,
      0.f, 0.f, q, 1.f,
      0.f, 0.f, -m_near * q, 0.f);
}

void Camera::SetPosition(mathz::Vec3&& pos)
{
  m_position = pos;

  m_forward = m_forward - pos;

  // Calculate the up vector in relation to the new camera direction
  mathz::Vec3 tmp(0.f, 1.f, 0.f);
  m_right = tmp.cross(m_forward);
  m_right.normalize();

  m_up = m_forward.cross(m_right);
  m_up.normalize();

  // Set up camera direction matrix
  m_transform[0][0] = m_right.x;
  m_transform[0][1] = m_right.y;
  m_transform[0][2] = m_right.z;
  m_transform[0][3] = 0.0f;
  m_transform[1][0] = m_up.x;
  m_transform[1][1] = m_up.y;
  m_transform[1][2] = m_up.z;
  m_transform[1][3] = 0.0f;
  m_transform[2][0] = m_forward.x;
  m_transform[2][1] = m_forward.y;
  m_transform[2][2] = m_forward.z;
  m_transform[2][3] = 0.0f;
  m_transform[3][0] = m_position.x;
  m_transform[3][1] = m_position.y;
  m_transform[3][2] = m_position.z;
  m_transform[3][3] = 1.0f;

  m_forward.normalize();
}

const mathz::Vec3& Camera::GetPosition() const
{
  return m_position;
}
