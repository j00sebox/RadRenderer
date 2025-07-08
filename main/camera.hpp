#pragma once

#include "../mathz/matrix.hpp"
#include "quaternion.hpp"

#include <SFML/Window/Keyboard.hpp>

class Camera
{
public:
  Camera() = delete;
  Camera(int width, int height, float near, float far, float fov);
  ~Camera() {}

  void move(const mathz::Vec3& pos);
  void rotate(float pitch, float yaw);

  const mathz::Vec3& getPosition() const { return m_position; }
  inline const mathz::Vec3& getForward() const { return m_forward; }
  inline const mathz::Vec3& getRight() const { return m_right; }
  const mathz::Mat4& getTransform() const { return m_transform; }
  const mathz::Mat4& getPerspective() const { return m_perspective; }

private:
  mathz::Vec3 m_position;
  mathz::Quaternion m_rotation;
  mathz::Vec3 m_forward, m_up, m_right;
  float m_near, m_far, m_fov;
  mathz::Mat4 m_transform;
  mathz::Mat4 m_perspective;
};
