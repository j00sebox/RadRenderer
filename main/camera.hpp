#pragma once

#include "../mathz/matrix.hpp"
#include "quaternion.hpp"

#include <SFML/Window/Keyboard.hpp>

struct CameraSettings
{
  int width;
  int height;
  float near;
  float far;
  float fov;
};

class Camera
{
public:
  Camera() = delete;
  Camera(CameraSettings&& camera_settings);
  ~Camera() {}

  void Move(mathz::Vec3&& pos);
  void Rotate(float pitch, float yaw);

  const mathz::Vec3& GetPosition() const { return m_position; }
  inline const mathz::Vec3& GetForward() const { return m_forward; }
  inline const mathz::Vec3& GetRight() const { return m_right; }
  const mathz::Mat4& GetTransform() const { return m_transform; }
  const mathz::Mat4& GetPerspective() const { return m_perspective; }

private:
  mathz::Vec3 m_position;
  mathz::Quaternion m_rotation;
  mathz::Vec3 m_forward, m_up, m_right;
  float m_near, m_far, m_fov;
  mathz::Mat4 m_transform;
  mathz::Mat4 m_perspective;
};
