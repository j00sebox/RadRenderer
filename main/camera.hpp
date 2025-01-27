#pragma once

#include "../mathz/matrix.hpp"
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

  mathz::Mat4 LookAt(mathz::Vec3 eye, const mathz::Vec3& target, const mathz::Vec3& up) const
  {

    mathz::Vec3 z = (target - eye); // Forward
    z.Normalize();
    mathz::Vec3 x = up.Cross(z); // Right
    x.Normalize();
    mathz::Vec3 y = z.Cross(x); // Up

    mathz::Mat4 mat;
    mat[0][0] = x.x;
    mat[1][0] = x.y;
    mat[2][0] = x.z;
    mat[0][1] = y.x;
    mat[1][1] = y.y;
    mat[2][1] = y.z;
    mat[0][2] = z.x;
    mat[1][2] = z.y;
    mat[2][2] = z.z;
    mat[3][0] = -x.Dot(eye);
    mat[3][1] = -y.Dot(eye);
    mat[3][2] = -z.Dot(eye);

    return mat;
  }

  const mathz::Vec3& GetPosition() const { return m_position; }
  inline const mathz::Vec3& GetForward() const { return m_forward; }
  inline const mathz::Vec3& GetRight() const { return m_right; }
  const mathz::Mat4& GetTransform() const { return m_transform; }
  const mathz::Mat4& GetPerspective() const { return m_perspective; }

private:
  mathz::Vec3 m_position;
  mathz::Vec3 m_forward, m_up, m_right;
  float m_near, m_far, m_fov;
  mathz::Mat4 m_transform;
  mathz::Mat4 m_perspective;
};
