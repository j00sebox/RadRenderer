#include "../mathz/quaternion.hpp"
#include "pch.h"
#include "renderer.hpp"

#include <cstdint>
#include <cstring>

Renderer::Renderer(unsigned int screen_width, unsigned int screen_height, float near, float far)

    : m_screen_width(screen_width), m_screen_height(screen_height),
      m_half_width(screen_width / 2), m_half_height(screen_height / 2),
      m_buffer_size(screen_width * screen_height * 4), // RGBA
      m_near(near), m_far(far),
      m_depth_buffer(m_buffer_size, -9999)
{
  m_frame_buffer.reset(new std::uint8_t[m_buffer_size]);

  m_directional_light = {0.0f, 4.0f, -1.0f};
  m_directional_light.Normalize();
}

bool OutOfBounds(const Triangle& t)
{
  return (t.vertices[0].x < -1.f || t.vertices[0].x > 1.f) ||
         (t.vertices[1].x < -1.f || t.vertices[1].x > 1.f) ||
         (t.vertices[2].x < -1.f || t.vertices[2].x > 1.f) ||

         (t.vertices[0].y < -1.f || t.vertices[0].y > 1.f) ||
         (t.vertices[1].y < -1.f || t.vertices[1].y > 1.f) ||
         (t.vertices[2].y < -1.f || t.vertices[2].y > 1.f) ||

         (t.vertices[0].z < -1.f || t.vertices[0].z > 1.f) ||
         (t.vertices[1].z < -1.f || t.vertices[1].z > 1.f) ||
         (t.vertices[2].z < -1.f || t.vertices[2].z > 1.f);
}

void Renderer::Render(const Model& model, const Camera& camera)
{
  // Clear screen to redraw
  ClearFrameBuffer();

  // Camera transform
  mathz::Mat4 cam_transform = camera.GetTransform();
  m_view = cam_transform.Inverse();

  // Iterate through all triangles in the object
  for (Triangle triangle : model)
  {
    // Apply model transform
    TransformTriangle(triangle, model.GetTransform());

    // Convert to camera space
    TransformTriangle(triangle, m_view);

    if (OutNearFarBounds(triangle))
      continue;

    // Remove when normals are attributes
    triangle.normal[0] = CalculateNormal(triangle);
    triangle.normal[1] = triangle.normal[0];
    triangle.normal[2] = triangle.normal[0];

    // Check if triangle is visible
    if (camera.GetForward().Dot(triangle.normal[0]) > 0 &&
        camera.GetForward().Dot(triangle.normal[1]) > 0 &&
        camera.GetForward().Dot(triangle.normal[2]) > 0)
    {
      triangle.z[0] = -triangle.vertices[0].z;
      triangle.z[1] = -triangle.vertices[1].z;
      triangle.z[2] = -triangle.vertices[2].z;

      TransformTriangle(triangle, camera.GetPerspective());

      if (OutOfBounds(triangle))
        continue;

      bool clipped = false;

      // First parameter is plane point, second is normal
      clipped |= ClipTriangle({-1.f, 0.f, 0.f}, {1.f, 0.f, 0.f}, triangle); // Left plane
      clipped |= ClipTriangle({1.f, 0.f, 0.f}, {-1.f, 0.f, 0.f}, triangle); // Right plane
      clipped |= ClipTriangle({0.f, 1.f, 0.f}, {0.f, -1.f, 0.f}, triangle); // Top plane
      clipped |= ClipTriangle({0.f, -1.f, 0.f}, {0.f, 1.f, 0.f}, triangle); // Bottom plane

      if (!clipped)
        m_render_triangles.push_back(triangle);
    }
  }

  for (const auto& t : m_clipped_tris)
  {
    m_render_triangles.push_back(t);
  }

  for (const auto& t : m_render_triangles)
  {
    Rasterize(t);
  }

  // Vectors needs to be empty for next redraw
  m_render_triangles.clear();
  m_clipped_tris.clear();
  ClearDepthBuffer();
}

void Renderer::Rasterize(const Triangle& t)
{
  int min_x, max_x;
  int min_y, max_y;

  mathz::Vec2<int> v0 = {ImageToScreenSpace(t.vertices[0].x, t.vertices[0].y)};
  mathz::Vec2<int> v1 = {ImageToScreenSpace(t.vertices[1].x, t.vertices[1].y)};
  mathz::Vec2<int> v2 = {ImageToScreenSpace(t.vertices[2].x, t.vertices[2].y)};

  // Bounding box
  min_x = std::min(v0.x, v1.x);
  min_x = std::min(min_x, v2.x);

  max_x = std::max(v0.x, v1.x);
  max_x = std::max(max_x, v2.x);

  min_y = std::min(v0.y, v1.y);
  min_y = std::min(min_y, v2.y);

  max_y = std::max(v0.y, v1.y);
  max_y = std::max(max_y, v2.y);

  for (int y = min_y; y < max_y; y++)
  {
    for (int x = min_x; x < max_x; x++)
    {
      mathz::Vec2<float> p = {x + 0.5f, y + 0.5f};

      float area0 = EdgeFunction((float)v0.x, (float)v0.y, (float)v1.x, (float)v1.y, p.x, p.y);
      float area1 = EdgeFunction((float)v1.x, (float)v1.y, (float)v2.x, (float)v2.y, p.x, p.y);
      float area2 = EdgeFunction((float)v2.x, (float)v2.y, (float)v0.x, (float)v0.y, p.x, p.y);

      if (area0 >= 0 &&
          area1 >= 0 &&
          area2 >= 0)
      {

        float area_t = EdgeFunction((float)v0.x, (float)v0.y, (float)v1.x, (float)v1.y, (float)v2.x, (float)v2.y);

        // Barycentric coordinates
        float l0 = area0 / area_t;
        float l1 = area1 / area_t;
        float l2 = area2 / area_t;

        float int_z = l0 * t.z[0] + l1 * t.z[1] + l2 * t.z[2];

        mathz::Vec3 normal = t.normal[0] * l0 + t.normal[1] * l1 + t.normal[2] * l2;

        float lum = m_directional_light.Dot(normal);

        // if (int_z > m_depth_buffer[y * m_screen_width + x])
        {
          SetPixel(x, y, GetColour(lum));

          // m_depth_buffer[y * m_screen_width + x] = int_z;
        }
      }
    }
  }
}

float Renderer::EdgeFunction(float x0, float y0, float x1, float y1, float x2, float y2)
{
  return ((x2 - x0) * (y1 - y0) - (y2 - y0) * (x1 - x0));
}

Pixel Renderer::GetColour(float lum)
{
  return {(std::uint8_t)(255), (std::uint8_t)(255), (std::uint8_t)(255), 255};
}

void Renderer::SetPixel(int x, int y, const Pixel& pixel)
{
  // Calculate the index of the pixel in the framebuffer
  int index = (y * m_screen_width + x) * 4;

  std::memcpy(&m_frame_buffer.get()[index], &pixel, sizeof(Pixel));
}

std::pair<int, int> Renderer::ImageToScreenSpace(float x, float y)
{
  std::cout << "xy (" << x << ", " << y << ")\n";

  x = m_half_width * x + m_half_width;
  y = m_half_height * y + m_half_height;

  return {(int)x, (int)y};
}

void Renderer::ClearFrameBuffer()
{
  std::memset(m_frame_buffer.get(), 0, m_buffer_size);
}

void Renderer::ClearDepthBuffer()
{
  for (auto& f : m_depth_buffer)
    f = -9999;
}

// Returns the point that the given plane and line intersect
mathz::Vec3 Renderer::LinePlaneIntersect(mathz::Vec3& point, mathz::Vec3& plane_normal, mathz::Vec3& line_begin, mathz::Vec3& line_end)
{
  // Using the equation for a plane Ax + Bx + Cx = D and line P(t) = P + (Q - P) *  t and solving for t
  float t = -(plane_normal.x * (line_begin.x - point.x) + plane_normal.y * (line_begin.y - point.y) + plane_normal.z * (line_begin.z - point.z)) /
            (plane_normal.x * (line_end.x - line_begin.x) + plane_normal.y * (line_end.y - line_begin.y) + plane_normal.z * (line_end.z - line_begin.z));

  mathz::Vec3 intersection_point = line_begin + (line_end - line_begin) * t;

  return intersection_point;
}

bool Renderer::ClipTriangle(mathz::Vec3&& plane_point, mathz::Vec3&& plane_normal, Triangle& t)
{
  // Make sure it's normalized
  plane_normal.Normalize();

  int in_verts = 0;
  int out_verts = 0;

  // This will keep track of which vertices are in vs out
  mathz::Vec3 in_vs[3];
  mathz::Vec3 out_vs[3];

  for (const auto& v : t.vertices)
  {
    mathz::Vec3 vert = {v.x, v.y, v.z};
    mathz::Vec3 line = vert - plane_point;
    line.Normalize();
    if (plane_normal.Dot(line) > 0)
    {
      in_vs[in_verts++] = v;
    }
    else
    {
      out_vs[out_verts++] = v;
    }
  }

  if (in_verts == 2)
  {
    Triangle t1, t2;

    t1.vertices[0] = in_vs[0];
    t1.vertices[1] = in_vs[1];

    t2.vertices[0] = in_vs[1];

    // The intersecting points to the plane will make up the rest of both triangles
    t1.vertices[2] = LinePlaneIntersect(plane_point, plane_normal, in_vs[0], out_vs[0]);

    t2.vertices[1] = LinePlaneIntersect(plane_point, plane_normal, in_vs[1], out_vs[0]);
    t2.vertices[2] = t1.vertices[2]; // Both new triangles share this vertex

    // Copy over attributes
    t1.z[0] = t.z[0];
    t1.z[1] = t.z[1];
    t1.z[2] = t.z[2];

    t1.normal[0] = t.normal[0];
    t1.normal[1] = t.normal[1];
    t1.normal[2] = t.normal[2];

    t2.z[0] = t.z[0];
    t2.z[1] = t.z[1];
    t2.z[2] = t.z[2];

    t2.normal[0] = t.normal[0];
    t2.normal[1] = t.normal[1];
    t2.normal[2] = t.normal[2];

    m_clipped_tris.push_back(t1);
    m_clipped_tris.push_back(t2);

    return true;
  }
  else if (in_verts == 1)
  {
    Triangle t1;
    t1.vertices[0] = in_vs[0];

    t1.vertices[1] = LinePlaneIntersect(plane_point, plane_normal, in_vs[0], out_vs[0]);
    t1.vertices[2] = LinePlaneIntersect(plane_point, plane_normal, in_vs[0], out_vs[1]);

    t1.z[0] = t.z[0];
    t1.z[1] = t.z[1];
    t1.z[2] = t.z[2];

    t1.normal[0] = t.normal[0];
    t1.normal[1] = t.normal[1];
    t1.normal[2] = t.normal[2];

    m_clipped_tris.push_back(t1);

    return true;
  }
  else if (in_verts == 0)
  {
    return true;
  }

  return false;
}

bool Renderer::OutNearFarBounds(const Triangle& t)
{
  return (t.vertices[0].z < m_near || t.vertices[0].z > m_far) ||
         (t.vertices[0].z < m_near || t.vertices[0].z > m_far) ||
         (t.vertices[0].z < m_near || t.vertices[0].z > m_far);
}

void Renderer::TransformTriangle(Triangle& t, const mathz::Mat4& transform)
{
  t.vertices[0] = transform * t.vertices[0];
  t.vertices[1] = transform * t.vertices[1];
  t.vertices[2] = transform * t.vertices[2];
}

mathz::Vec3 Renderer::CalculateNormal(Triangle& t)
{
  // Construct line 1 of the triangle
  mathz::Vec3 l0 = {
      t.vertices[1].x - t.vertices[0].x,
      t.vertices[1].y - t.vertices[0].y,
      t.vertices[1].z - t.vertices[0].z};

  // Construct line 2 of the triangle
  mathz::Vec3 l1 = {
      t.vertices[2].x - t.vertices[0].x,
      t.vertices[2].y - t.vertices[0].y,
      t.vertices[2].z - t.vertices[0].z};

  mathz::Vec3 face_normal = l1.Cross(l0);
  face_normal.Normalize();

  return face_normal;
}
