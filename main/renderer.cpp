#include "pch.h"
#include "renderer.hpp"
#include "vector.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>

Renderer::Renderer(unsigned int screen_width, unsigned int screen_height, float near, float far)

    : m_screen_width(screen_width), m_screen_height(screen_height),
      m_buffer_size(screen_width * screen_height * 4), // RGBA
      m_near(near), m_far(far)
{
  m_frame_buffer.reset(new std::uint8_t[m_buffer_size]);

  m_directional_light = { 0.f, 4.f, -1.f };
  m_directional_light.Normalize();
}

void Renderer::Render(const Model& model, Camera& camera)
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

    triangle.normal[0] = CalculateNormal(triangle);
    triangle.normal[1] = triangle.normal[0];
    triangle.normal[2] = triangle.normal[0];

    // Check if triangle is visible
    if (triangle.normal[0].Dot(triangle.Center()) >= 0)
    {
      bool clipped = false;

      // Clip on near and far planes
      // First parameter is plane point, second is normal
      clipped |= ClipTriangle({0.f, 0.f, -m_near}, {0.f, 0.f, -1.f}, triangle); // Front plane
      clipped |= ClipTriangle({0.f, 0.f, -m_far}, {0.f, 0.f, 1.f}, triangle);   // Back plane

      if (!clipped)
      {
        TransformTriangle(triangle, camera.GetPerspective());
        m_render_triangles.push_back(triangle);
      }
    }
  }

  for (Triangle& triangle : m_clipped_triangles)
  {
    TransformTriangle(triangle, camera.GetPerspective());
    m_render_triangles.push_back(triangle);
  }

  std::sort(m_render_triangles.begin(), m_render_triangles.end(), [](const Triangle& t1, const Triangle& t2)
            {
              float t1_average_z = (t1.vertices[0].z + t1.vertices[1].z + t1.vertices[2].z) / 3.f;
              float t2_average_z = (t2.vertices[0].z + t2.vertices[1].z + t2.vertices[2].z) / 3.f;

              return t1_average_z < t2_average_z; });

  for (const auto& t : m_render_triangles)
  {
    Rasterize(t);
  }

  // Vectors needs to be empty for next redraw
  m_render_triangles.clear();
  m_clipped_triangles.clear();
}

Pixel Renderer::GetColour(float lum)
{
    Pixel p = { (std::uint8_t)(255 * cosf(lum) * m_diffuse_constant), (std::uint8_t)(255 * cosf(lum) * m_diffuse_constant), (std::uint8_t)(255 * cosf(lum) * m_diffuse_constant), 255 };
    return p;
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

        mathz::Vec3 normal = t.normal[0] * l0 + t.normal[1] * l1 + t.normal[2] * l2;
     
        float lum = normal.Dot(m_directional_light);
        SetPixel(x, y, GetColour(lum));
      }
    }
  }
}

float Renderer::EdgeFunction(float x0, float y0, float x1, float y1, float x2, float y2)
{
  return ((x2 - x0) * (y1 - y0) - (y2 - y0) * (x1 - x0));
}

void Renderer::SetPixel(int x, int y, const Pixel& pixel)
{
  // Calculate the index of the pixel in the framebuffer
  int index = (y * m_screen_width + x) * 4;

  std::memcpy(&m_frame_buffer.get()[index], &pixel, sizeof(Pixel));
}

std::pair<int, int> Renderer::ImageToScreenSpace(float x, float y)
{
  // NDC coordinates are in range [-1, 1]
  // Screen coordinates are in range [0, screenWidth] for x and [0, screenHeight] for y
  const float normalized_x = (x + 1.0) * 0.5;
  const float normalized_y = (y + 1.0) * 0.5;

  // Then scale to screen dimensions
  // Note: Y is flipped because screen coordinates typically have 0 at the top
  const int screen_x = std::clamp((int)std::floor(normalized_x * m_screen_width), 0, (int)m_screen_width);
  const int screen_y = std::clamp((int)std::floor((1.0 - normalized_y) * m_screen_height), 0, (int)m_screen_height);

  return {screen_x, screen_y};
}

void Renderer::ClearFrameBuffer()
{
  std::memset(m_frame_buffer.get(), 0, m_buffer_size);
}

// Returns the point that the given plane and line intersect
mathz::Vec3 Renderer::LinePlaneIntersect(const mathz::Vec3& point, const mathz::Vec3& plane_normal, mathz::Vec3& line_begin, mathz::Vec3& line_end)
{
  // Using the equation for a plane Ax + Bx + Cx = D and line P(t) = P + (Q - P) *  t and solving for t
  float t = -(plane_normal.x * (line_begin.x - point.x) + plane_normal.y * (line_begin.y - point.y) + plane_normal.z * (line_begin.z - point.z)) /
            (plane_normal.x * (line_end.x - line_begin.x) + plane_normal.y * (line_end.y - line_begin.y) + plane_normal.z * (line_end.z - line_begin.z));

  mathz::Vec3 intersection_point = line_begin + (line_end - line_begin) * t;

  return intersection_point;
}

bool Renderer::ClipTriangle(const mathz::Vec3& plane_point, const mathz::Vec3& plane_normal, Triangle& t)
{
  int in_count = 0;
  int out_count = 0;

  // This will keep track of which vertices are in vs out
  mathz::Vec3 in_vertices[3];
  mathz::Vec3 out_vertices[3];

  for (const mathz::Vec3& vertex : t.vertices)
  {
    mathz::Vec3 line = vertex - plane_point;
    if (plane_normal.Dot(line) >= 0)
    {
      in_vertices[in_count++] = vertex;
    }
    else
    {
      out_vertices[out_count++] = vertex;
    }
  }

  if (in_count == 2)
  {
    Triangle t1, t2;

    t1.vertices[0] = in_vertices[0];
    t1.vertices[1] = in_vertices[1];

    t2.vertices[0] = in_vertices[1];

    // The intersecting points to the plane will make up the rest of both triangles
    t1.vertices[2] = LinePlaneIntersect(plane_point, plane_normal, in_vertices[0], out_vertices[0]);

    t2.vertices[1] = LinePlaneIntersect(plane_point, plane_normal, in_vertices[1], out_vertices[0]);
    t2.vertices[2] = t1.vertices[2]; // Both new triangles share this vertex

    t1.normal[0] = t.normal[0];
    t1.normal[1] = t.normal[1];
    t1.normal[2] = t.normal[2];

    t2.normal[0] = t.normal[0];
    t2.normal[1] = t.normal[1];
    t2.normal[2] = t.normal[2];

    m_clipped_triangles.push_back(t1);
    m_clipped_triangles.push_back(t2);

    return true;
  }
  else if (in_count == 1)
  {
    Triangle t1;
    t1.vertices[0] = in_vertices[0];

    t1.vertices[1] = LinePlaneIntersect(plane_point, plane_normal, in_vertices[0], out_vertices[0]);
    t1.vertices[2] = LinePlaneIntersect(plane_point, plane_normal, in_vertices[0], out_vertices[1]);

    t1.normal[0] = t.normal[0];
    t1.normal[1] = t.normal[1];
    t1.normal[2] = t.normal[2];

    m_clipped_triangles.push_back(t1);

    return true;
  }
  else if (in_count == 0)
  {
    return true;
  }

  return false;
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
