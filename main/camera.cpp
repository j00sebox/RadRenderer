#include "camera.hpp"
#include "pch.h"

#include "../mathz/misc.hpp"
#include <cmath>

Camera::Camera(int width, int height, float near, float far, float fov)
    : m_near(near),
      m_far(far),
      m_fov(fov)
{
  m_forward = {0.f, 0.f, 1.f};
  m_up = {0.f, 1.f, 0.f};
  m_right = {1.f, 0.f, 0.f};

  float cotan_half_fov = 1.0f / std::tan(mathz::Radians(m_fov * 0.5f));
  float aspect_ratio_inverse = (float)height / (float)width;

  // Create projection matrix
  m_perspective[0][0] = cotan_half_fov * aspect_ratio_inverse;
  m_perspective[1][1] = cotan_half_fov;
  m_perspective[2][2] = (m_far + m_near) / (m_far - m_near);
  m_perspective[2][3] = 2 * m_far * m_near / (m_far - m_near);
  m_perspective[3][2] = -1.f;
  m_perspective[3][3] = 0.f;

  Move({0.f, 3.f, 0.f});
  Rotate(0.f, 0.f);
}

void Camera::Move(const mathz::Vec3& postion)
{
  m_position = std::move(postion);

  m_transform[0][3] = m_position.x;
  m_transform[1][3] = m_position.y;
  m_transform[2][3] = m_position.z;
  m_transform[3][3] = 1.0f;
}

void Camera::Rotate(float pitch, float yaw)
{
  float cos_pitch = cos(mathz::Radians(pitch));

  float sin_pitch = sin(mathz::Radians(pitch));

  float cos_yaw = cos(mathz::Radians(yaw));

  float sin_yaw = sin(mathz::Radians(yaw));

  m_forward = {sin_yaw * cos_pitch, sin_pitch, -cos_pitch * cos_yaw};
  m_forward.Normalize();

  // Recalculate right and up vectors
  mathz::Vec3 world_up(0.f, 1.f, 0.f);
  m_right = world_up.Cross(m_forward);
  m_right.Normalize();

  m_up = m_forward.Cross(m_right);
  m_up.Normalize();

  // Update the transformation matrix
  m_transform[0][0] = m_right.x;
  m_transform[1][0] = m_right.y;
  m_transform[2][0] = m_right.z;

  m_transform[0][1] = m_up.x;
  m_transform[1][1] = m_up.y;
  m_transform[2][1] = m_up.z;

  m_transform[0][2] = m_forward.x;
  m_transform[1][2] = m_forward.y;
  m_transform[2][2] = m_forward.z;
}
