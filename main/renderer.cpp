#include "pch.h"
#include "renderer.hpp"
#include "vector.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <xmmintrin.h>  
#include <emmintrin.h>  

Renderer::Renderer(unsigned int screen_width, unsigned int screen_height, float near, float far)
    : m_screen_width(screen_width), m_screen_height(screen_height),
      m_buffer_size(screen_width * screen_height * 4),
      m_near(near), m_far(far),
      m_depth_buffer(screen_width * screen_height, -1.f)
{
  m_frame_buffer.reset(new std::uint8_t[m_buffer_size]);

  m_directional_light = { 0.f, 1.f, 0.f };
  m_directional_light.normalize();

  float half_fov_y = g_camera->getFov() * 0.5f * (3.14159265f / 180.f);
  float half_fov_x = std::atan(std::tan(half_fov_y) * (float)m_screen_width / (float)m_screen_height);
  float cx = std::cos(half_fov_x), sx = std::sin(half_fov_x);
  float cy = std::cos(half_fov_y), sy = std::sin(half_fov_y);
  m_frustum_planes[0] = { -cx, 0.f, -sx };  // Right
  m_frustum_planes[1] = {  cx, 0.f, -sx };  // Left
  m_frustum_planes[2] = { 0.f, -cy, -sy };  // Top
  m_frustum_planes[3] = { 0.f,  cy, -sy };  // Bottom

  m_tiles_x = (screen_width + TILE_SIZE - 1) / TILE_SIZE;
  m_tiles_y = (screen_height + TILE_SIZE - 1) / TILE_SIZE;
  m_tiles.resize(m_tiles_x * m_tiles_y);

  for (unsigned int ty = 0; ty < m_tiles_y; ++ty)
  {
    for (unsigned int tx = 0; tx < m_tiles_x; ++tx)
    {
      Tile& tile = m_tiles[ty * m_tiles_x + tx];
      tile.x0 = tx * TILE_SIZE;
      tile.y0 = ty * TILE_SIZE;
      tile.x1 = std::min(tile.x0 + TILE_SIZE, (int)screen_width);
      tile.y1 = std::min(tile.y0 + TILE_SIZE, (int)screen_height);
    }
  }

  // Start thread pool
  m_pool_size = std::thread::hardware_concurrency();
  if (m_pool_size == 0) m_pool_size = 4;

  for (unsigned int i = 0; i < m_pool_size; ++i)
  {
    m_thread_pool.emplace_back([this]()
    {
      unsigned int last_epoch = 0;
      while (true)
      {
        std::function<void()> task;
        {
          std::unique_lock<std::mutex> lock(m_pool_mutex);
          m_pool_cv.wait(lock, [this, &last_epoch]() { return m_pool_shutdown || m_pool_epoch != last_epoch; });
          if (m_pool_shutdown) return;
          last_epoch = m_pool_epoch;
          task = m_pool_task;
        }
        task();
        if (m_pool_finished.fetch_add(1) + 1 == m_pool_size)
          m_pool_done_cv.notify_one();
      }
    });
  }
}

Renderer::~Renderer()
{
  {
    std::unique_lock<std::mutex> lock(m_pool_mutex);
    m_pool_shutdown = true;
  }
  m_pool_cv.notify_all();
  for (auto& t : m_thread_pool)
    t.join();
}

void Renderer::render(const Model& model)
{
  Camera& camera = *g_camera;
  clearFrameBuffer();

  const MeshData& model_mesh = model.getMeshData();
  size_t triangle_count = model_mesh.triangleCount();
  size_t vertex_count = model_mesh.vertices.size();

  if (m_render_mesh.vertices.size() != vertex_count)
  {
    m_render_mesh.vertices.resize(vertex_count);
    m_render_mesh.normals.resize(vertex_count);
    m_render_mesh.z.resize(vertex_count);
    m_visible.resize(triangle_count);
  }

  mathz::Mat4 cam_transform = camera.getTransform();
  m_view = cam_transform.inverse();
  mathz::Mat4 model_view = m_view * model.getTransform();

  // Transform directional light into view space (direction only, upper 3x3)
  m_view_light = {
    m_view[0][0] * m_directional_light.x + m_view[0][1] * m_directional_light.y + m_view[0][2] * m_directional_light.z,
    m_view[1][0] * m_directional_light.x + m_view[1][1] * m_directional_light.y + m_view[1][2] * m_directional_light.z,
    m_view[2][0] * m_directional_light.x + m_view[2][1] * m_directional_light.y + m_view[2][2] * m_directional_light.z,
  };
  m_view_light.normalize();

  transformVertices(model_mesh.vertices, m_render_mesh, model_view);

  m_clipped_mesh.clear();

  for (size_t i = 0; i < triangle_count; ++i)
  {
    size_t base = i * 3;

    const mathz::Vec3& v0 = m_render_mesh.vertices[base];
    const mathz::Vec3& v1 = m_render_mesh.vertices[base + 1];
    const mathz::Vec3& v2 = m_render_mesh.vertices[base + 2];

    // Reject if all 3 vertices are outside any single frustum side plane
    bool outside = false;
    for (const mathz::Vec3& n : m_frustum_planes)
    {
      if (n.dot(v0) < 0.f && n.dot(v1) < 0.f && n.dot(v2) < 0.f)
      {
        outside = true;
        break;
      }
    }
    if (outside)
    {
      m_visible[i] = false;
      continue;
    }

    mathz::Vec3 normal = calculateNormal(i, m_render_mesh);
    m_render_mesh.normals[base] = normal;
    m_render_mesh.normals[base + 1] = normal;
    m_render_mesh.normals[base + 2] = normal;

    mathz::Vec3 centroid = {
      (v0.x + v1.x + v2.x) / 3.f,
      (v0.y + v1.y + v2.y) / 3.f,
      (v0.z + v1.z + v2.z) / 3.f
    };

    if (normal.dot(centroid) < 0)
    {
      m_visible[i] = false;
      continue;
    }

    m_visible[i] = true;

    // Clip only triangles that passed backface cull
    bool clipped = false;
    clipped |= clipTriangle({0.f, 0.f, -m_near}, {0.f, 0.f, -1.f}, i, m_render_mesh, model_mesh);
    clipped |= clipTriangle({0.f, 0.f, -m_far}, {0.f, 0.f, 1.f}, i, m_render_mesh, model_mesh);

    if (clipped)
      m_visible[i] = false;
  }

  transformVertices(m_clipped_mesh, camera.getPerspective());
  transformVertices(m_render_mesh, camera.getPerspective());

  for (size_t i = 0; i < m_clipped_mesh.triangleCount(); ++i)
  {
    size_t base = i * 3;

    mathz::Vec3 ab = m_clipped_mesh.vertices[base + 1] - m_clipped_mesh.vertices[base];
    mathz::Vec3 ac = m_clipped_mesh.vertices[base + 2] - m_clipped_mesh.vertices[base];
    float sign = ab.x * ac.y - ac.x * ab.y;

    if (sign < 0)
    {
      m_clipped_mesh.z[base] = -999.f; 
      continue;
    }

    m_clipped_mesh.z[base] = m_clipped_mesh.vertices[base].z;
    m_clipped_mesh.z[base + 1] = m_clipped_mesh.vertices[base + 1].z;
    m_clipped_mesh.z[base + 2] = m_clipped_mesh.vertices[base + 2].z;
  }

  for (size_t i = 0; i < triangle_count; ++i)
  {
    if (!m_visible[i])
      continue;

    size_t base = i * 3;

    mathz::Vec3 ab = m_render_mesh.vertices[base + 1] - m_render_mesh.vertices[base];
    mathz::Vec3 ac = m_render_mesh.vertices[base + 2] - m_render_mesh.vertices[base];
    float sign = ab.x * ac.y - ac.x * ab.y;

    if (sign < 0)
    {
      m_visible[i] = false;
      continue;
    }

    m_render_mesh.z[base] = m_render_mesh.vertices[base].z;
    m_render_mesh.z[base + 1] = m_render_mesh.vertices[base + 1].z;
    m_render_mesh.z[base + 2] = m_render_mesh.vertices[base + 2].z;
  }

  binTriangles();

  const auto& textures = model.getTextures();
  m_triangles_rendered = 0;

  std::atomic<size_t> next_tile{0};
  size_t total_tiles = m_tiles.size();

  {
    std::unique_lock<std::mutex> lock(m_pool_mutex);
    m_pool_finished = 0;
    m_pool_task = [this, &next_tile, total_tiles, &model_mesh, &textures]()
    {
      while (true)
      {
        size_t tile_idx = next_tile.fetch_add(1);
        if (tile_idx >= total_tiles)
          break;
        renderTile(tile_idx, model_mesh, textures);
      }
    };
    ++m_pool_epoch;
  }
  m_pool_cv.notify_all();

  {
    std::unique_lock<std::mutex> lock(m_pool_mutex);
    m_pool_done_cv.wait(lock, [this]() { return m_pool_finished == m_pool_size; });
  }

  m_stats.triangles_rendered = m_triangles_rendered;
}

void Renderer::transformVertices(MeshData& mesh, const mathz::Mat4& transform)
{
  __m128 col0 = _mm_setr_ps(transform[0][0], transform[1][0], transform[2][0], transform[3][0]);
  __m128 col1 = _mm_setr_ps(transform[0][1], transform[1][1], transform[2][1], transform[3][1]);
  __m128 col2 = _mm_setr_ps(transform[0][2], transform[1][2], transform[2][2], transform[3][2]);
  __m128 col3 = _mm_setr_ps(transform[0][3], transform[1][3], transform[2][3], transform[3][3]);

  for (size_t i = 0; i < mesh.vertices.size(); ++i)
  {
    mathz::Vec3& v = mesh.vertices[i];

    __m128 vx = _mm_set1_ps(v.x);
    __m128 vy = _mm_set1_ps(v.y);
    __m128 vz = _mm_set1_ps(v.z);

    __m128 result = _mm_add_ps(
        _mm_add_ps(_mm_mul_ps(col0, vx), _mm_mul_ps(col1, vy)),
        _mm_add_ps(_mm_mul_ps(col2, vz), col3)
    );

    alignas(16) float out[4];
    _mm_store_ps(out, result);

    if (out[3] != 0.0f)
    {
      float inv_w = 1.0f / out[3];
      v.x = out[0] * inv_w;
      v.y = out[1] * inv_w;
      v.z = out[2] * inv_w;
    }
    else
    {
      v.x = out[0];
      v.y = out[1];
      v.z = out[2];
    }
  }
}

void Renderer::transformVertices(const std::vector<mathz::Vec3>& src, MeshData& dst, const mathz::Mat4& transform)
{
  __m128 col0 = _mm_setr_ps(transform[0][0], transform[1][0], transform[2][0], transform[3][0]);
  __m128 col1 = _mm_setr_ps(transform[0][1], transform[1][1], transform[2][1], transform[3][1]);
  __m128 col2 = _mm_setr_ps(transform[0][2], transform[1][2], transform[2][2], transform[3][2]);
  __m128 col3 = _mm_setr_ps(transform[0][3], transform[1][3], transform[2][3], transform[3][3]);

  for (size_t i = 0; i < src.size(); ++i)
  {
    const mathz::Vec3& v = src[i];

    __m128 vx = _mm_set1_ps(v.x);
    __m128 vy = _mm_set1_ps(v.y);
    __m128 vz = _mm_set1_ps(v.z);

    __m128 result = _mm_add_ps(
        _mm_add_ps(_mm_mul_ps(col0, vx), _mm_mul_ps(col1, vy)),
        _mm_add_ps(_mm_mul_ps(col2, vz), col3)
    );

    alignas(16) float out[4];
    _mm_store_ps(out, result);

    mathz::Vec3& d = dst.vertices[i];
    if (out[3] != 0.0f)
    {
      float inv_w = 1.0f / out[3];
      d.x = out[0] * inv_w;
      d.y = out[1] * inv_w;
      d.z = out[2] * inv_w;
    }
    else
    {
      d.x = out[0];
      d.y = out[1];
      d.z = out[2];
    }
  }
}

mathz::Vec3 Renderer::calculateNormal(size_t tri_idx, const MeshData& mesh)
{
  size_t base = tri_idx * 3;

  mathz::Vec3 l0 = {
      mesh.vertices[base + 1].x - mesh.vertices[base].x,
      mesh.vertices[base + 1].y - mesh.vertices[base].y,
      mesh.vertices[base + 1].z - mesh.vertices[base].z
  };

  mathz::Vec3 l1 = {
      mesh.vertices[base + 2].x - mesh.vertices[base].x,
      mesh.vertices[base + 2].y - mesh.vertices[base].y,
      mesh.vertices[base + 2].z - mesh.vertices[base].z
  };

  mathz::Vec3 face_normal = l1.cross(l0);
  face_normal.normalize();

  return face_normal;
}

Pixel Renderer::getColour(float lum)
{
    Pixel p = { (std::uint8_t)(255 * cosf(lum) * m_diffuse_constant), (std::uint8_t)(255 * cosf(lum) * m_diffuse_constant), (std::uint8_t)(255 * cosf(lum) * m_diffuse_constant), 255 };
    return p;
}

void Renderer::rasterize(size_t tri_idx, const MeshData& transformed, const MeshData& source, const std::vector<Texture>& textures)
{
  size_t base = tri_idx * 3;

  mathz::Vec2<int> v0 = {imageToScreenSpace(transformed.vertices[base].x, transformed.vertices[base].y)};
  mathz::Vec2<int> v1 = {imageToScreenSpace(transformed.vertices[base + 1].x, transformed.vertices[base + 1].y)};
  mathz::Vec2<int> v2 = {imageToScreenSpace(transformed.vertices[base + 2].x, transformed.vertices[base + 2].y)};

  int min_x = std::min({v0.x, v1.x, v2.x});
  int max_x = std::max({v0.x, v1.x, v2.x});
  int min_y = std::min({v0.y, v1.y, v2.y});
  int max_y = std::max({v0.y, v1.y, v2.y});

  min_x = std::max(min_x, 0);
  min_y = std::max(min_y, 0);
  max_x = std::min(max_x, (int)m_screen_width);
  max_y = std::min(max_y, (int)m_screen_height);

  if (min_x >= max_x || min_y >= max_y)
    return;

  const Texture& texture = textures[source.materials[tri_idx]];

  const mathz::Vec3& n0 = transformed.normals[base];
  const mathz::Vec3& n1 = transformed.normals[base + 1];
  const mathz::Vec3& n2 = transformed.normals[base + 2];

  const mathz::Vec2<float>& uv0 = source.uvs[base];
  const mathz::Vec2<float>& uv1 = source.uvs[base + 1];
  const mathz::Vec2<float>& uv2 = source.uvs[base + 2];

  float tz0 = transformed.z[base];
  float tz1 = transformed.z[base + 1];
  float tz2 = transformed.z[base + 2];

  // Perspective-correct interpolation: recover 1/w from NDC z
  float pc_A = (m_far + m_near) / (m_far - m_near);
  float pc_B = 2.f * m_far * m_near / (m_far - m_near);
  float inv_w0 = (tz0 + pc_A) / pc_B;
  float inv_w1 = (tz1 + pc_A) / pc_B;
  float inv_w2 = (tz2 + pc_A) / pc_B;
  float u0w = uv0.x * inv_w0, v0w = uv0.y * inv_w0;
  float u1w = uv1.x * inv_w1, v1w = uv1.y * inv_w1;
  float u2w = uv2.x * inv_w2, v2w = uv2.y * inv_w2;

  float diffuse = -n0.dot(m_view_light);  // normals are inward, negate for outward
  if (diffuse < 0.f) diffuse = 0.f;
  float light = m_ambient + diffuse * m_diffuse_constant;
  if (light > 1.f) light = 1.f;

  // Convert to float for edge calculations
  float fx0 = (float)v0.x, fy0 = (float)v0.y;
  float fx1 = (float)v1.x, fy1 = (float)v1.y;
  float fx2 = (float)v2.x, fy2 = (float)v2.y;

  // Precompute edge function deltas (for incremental calculation)
  // Edge 0: v0 -> v1
  float e0_dx = fy1 - fy0;  // delta when x increases by 1
  float e0_dy = fx0 - fx1;  // delta when y increases by 1

  // Edge 1: v1 -> v2
  float e1_dx = fy2 - fy1;
  float e1_dy = fx1 - fx2;

  // Edge 2: v2 -> v0
  float e2_dx = fy0 - fy2;
  float e2_dy = fx2 - fx0;

  // Precompute total area and inverse (avoid division in inner loop)
  // Must match edgeFunction(v0, v1, v2): (v2-v0) x (v1-v0)
  float area_t = (fx2 - fx0) * (fy1 - fy0) - (fy2 - fy0) * (fx1 - fx0);
  if (area_t == 0.0f) return;
  float inv_area = 1.0f / area_t;

  // Compute edge values at starting corner (min_x + 0.5, min_y + 0.5)
  float start_x = min_x + 0.5f;
  float start_y = min_y + 0.5f;

  float e0_row = (start_x - fx0) * (fy1 - fy0) - (start_y - fy0) * (fx1 - fx0);
  float e1_row = (start_x - fx1) * (fy2 - fy1) - (start_y - fy1) * (fx2 - fx1);
  float e2_row = (start_x - fx2) * (fy0 - fy2) - (start_y - fy2) * (fx0 - fx2);

  // SSE constants for processing 4 pixels at once
  __m128 e0_dx4 = _mm_set1_ps(e0_dx * 4.0f);
  __m128 e1_dx4 = _mm_set1_ps(e1_dx * 4.0f);
  __m128 e2_dx4 = _mm_set1_ps(e2_dx * 4.0f);
  __m128 e0_dx_step = _mm_setr_ps(0, e0_dx, e0_dx * 2, e0_dx * 3);
  __m128 e1_dx_step = _mm_setr_ps(0, e1_dx, e1_dx * 2, e1_dx * 3);
  __m128 e2_dx_step = _mm_setr_ps(0, e2_dx, e2_dx * 2, e2_dx * 3);
  __m128 zeros = _mm_setzero_ps();

  for (int y = min_y; y < max_y; y++)
  {
    float e0 = e0_row;
    float e1 = e1_row;
    float e2 = e2_row;

    int x = min_x;

    // Process 4 pixels at a time with SSE
    for (; x + 4 <= max_x; x += 4)
    {
      // Load edge values for 4 consecutive pixels
      __m128 edge0 = _mm_add_ps(_mm_set1_ps(e0), e0_dx_step);
      __m128 edge1 = _mm_add_ps(_mm_set1_ps(e1), e1_dx_step);
      __m128 edge2 = _mm_add_ps(_mm_set1_ps(e2), e2_dx_step);

      // Check which pixels are inside triangle (all edges >= 0)
      __m128 mask0 = _mm_cmpge_ps(edge0, zeros);
      __m128 mask1 = _mm_cmpge_ps(edge1, zeros);
      __m128 mask2 = _mm_cmpge_ps(edge2, zeros);
      __m128 inside = _mm_and_ps(_mm_and_ps(mask0, mask1), mask2);

      int mask = _mm_movemask_ps(inside);

      if (mask != 0)
      {
        // At least one pixel is inside - process individually
        alignas(16) float e0_arr[4], e1_arr[4], e2_arr[4];
        _mm_store_ps(e0_arr, edge0);
        _mm_store_ps(e1_arr, edge1);
        _mm_store_ps(e2_arr, edge2);

        for (int i = 0; i < 4; ++i)
        {
          if (mask & (1 << i))
          {
            float w0 = e1_arr[i] * inv_area;
            float w1 = e2_arr[i] * inv_area;
            float w2 = e0_arr[i] * inv_area;

            float int_z = w0 * tz0 + w1 * tz1 + w2 * tz2;

            int px = x + i;
            int index = y * m_screen_width + px;

            if (int_z > m_depth_buffer[index])
            {
              float pc_w = w0 * inv_w0 + w1 * inv_w1 + w2 * inv_w2;
              float pc_u = (w0 * u0w + w1 * u1w + w2 * u2w) / pc_w;
              float pc_v = (w0 * v0w + w1 * v1w + w2 * v2w) / pc_w;
              Colour colour = texture.sample(pc_u, pc_v);

              Pixel pixel = {
                std::uint8_t(colour.r * light * 255),
                std::uint8_t(colour.g * light * 255),
                std::uint8_t(colour.b * light * 255),
                std::uint8_t(colour.a * 255)
              };

              int fb_index = index * 4;
              std::memcpy(&m_frame_buffer.get()[fb_index], &pixel, sizeof(Pixel));
              m_depth_buffer[index] = int_z;
            }
          }
        }
      }

      e0 += e0_dx * 4;
      e1 += e1_dx * 4;
      e2 += e2_dx * 4;
    }

    // Handle remaining pixels (less than 4)
    for (; x < max_x; x++)
    {
      if (e0 >= 0 && e1 >= 0 && e2 >= 0)
      {
        float w0 = e1 * inv_area;
        float w1 = e2 * inv_area;
        float w2 = e0 * inv_area;

        float int_z = w0 * tz0 + w1 * tz1 + w2 * tz2;

        int index = y * m_screen_width + x;

        if (int_z > m_depth_buffer[index])
        {
          float pc_w = w0 * inv_w0 + w1 * inv_w1 + w2 * inv_w2;
          float pc_u = (w0 * u0w + w1 * u1w + w2 * u2w) / pc_w;
          float pc_v = (w0 * v0w + w1 * v1w + w2 * v2w) / pc_w;
          Colour colour = texture.sample(pc_u, pc_v);

          Pixel pixel = {
            std::uint8_t(colour.r * light * 255),
            std::uint8_t(colour.g * light * 255),
            std::uint8_t(colour.b * light * 255),
            std::uint8_t(colour.a * 255)
          };

          int fb_index = index * 4;
          std::memcpy(&m_frame_buffer.get()[fb_index], &pixel, sizeof(Pixel));
          m_depth_buffer[index] = int_z;
        }
      }

      e0 += e0_dx;
      e1 += e1_dx;
      e2 += e2_dx;
    }

    // Move to next row
    e0_row += e0_dy;
    e1_row += e1_dy;
    e2_row += e2_dy;
  }
}

float Renderer::edgeFunction(float x0, float y0, float x1, float y1, float x2, float y2)
{
  return ((x2 - x0) * (y1 - y0) - (y2 - y0) * (x1 - x0));
}

void Renderer::setPixel(int x, int y, const Pixel& pixel)
{
  int index = (y * m_screen_width + x) * 4;
  std::memcpy(&m_frame_buffer.get()[index], &pixel, sizeof(Pixel));
}

std::pair<int, int> Renderer::imageToScreenSpace(float x, float y)
{
  const float normalized_x = (x + 1.0) * 0.5;
  const float normalized_y = (y + 1.0) * 0.5;

  const int screen_x = (int)std::floor(normalized_x * m_screen_width);
  const int screen_y = (int)std::floor((1.0 - normalized_y) * m_screen_height);

  return {screen_x, screen_y};
}

void Renderer::clearFrameBuffer()
{
  std::memset(m_frame_buffer.get(), 0, m_buffer_size);
  std::fill(m_depth_buffer.begin(), m_depth_buffer.end(), -1.0f);
}

mathz::Vec3 Renderer::linePlaneIntersect(const mathz::Vec3& point, const mathz::Vec3& plane_normal, mathz::Vec3& line_begin, mathz::Vec3& line_end, float& t)
{
  t = -(plane_normal.x * (line_begin.x - point.x) + plane_normal.y * (line_begin.y - point.y) + plane_normal.z * (line_begin.z - point.z)) /
      (plane_normal.x * (line_end.x - line_begin.x) + plane_normal.y * (line_end.y - line_begin.y) + plane_normal.z * (line_end.z - line_begin.z));

  return line_begin + (line_end - line_begin) * t;
}

bool Renderer::clipTriangle(const mathz::Vec3& plane_point, const mathz::Vec3& plane_normal, size_t tri_idx, MeshData& transformed, const MeshData& source)
{
  size_t base = tri_idx * 3;

  int in_count = 0;
  int out_count = 0;

  mathz::Vec3 in_vertices[3];
  mathz::Vec3 out_vertices[3];
  int in_indices[3];
  int out_indices[3];

  for (int i = 0; i < 3; ++i)
  {
    mathz::Vec3& vertex = transformed.vertices[base + i];
    mathz::Vec3 line = vertex - plane_point;

    if (plane_normal.dot(line) >= 0)
    {
      in_vertices[in_count] = vertex;
      in_indices[in_count] = i;
      ++in_count;
    }
    else
    {
      out_vertices[out_count] = vertex;
      out_indices[out_count] = i;
      ++out_count;
    }
  }

  // Preserve original winding order (v0 -> v1 -> v2)
  if (in_count == 1 && out_count == 2)
  {
    if (out_indices[0] != (in_indices[0] + 1) % 3)
    {
      std::swap(out_vertices[0], out_vertices[1]);
      std::swap(out_indices[0], out_indices[1]);
    }
  }
  else if (in_count == 2 && out_count == 1)
  {
    if (in_indices[0] != (out_indices[0] + 1) % 3)
    {
      std::swap(in_vertices[0], in_vertices[1]);
      std::swap(in_indices[0], in_indices[1]);
    }
  }

  if (in_count == 3)
  {
    return false; // No clipping needed
  }

  mathz::Vec3 normal = transformed.normals[base];
  int material = source.materials[tri_idx];

  // Get UVs for interpolation from source
  mathz::Vec2<float> uvs[3] = {
    source.uvs[base],
    source.uvs[base + 1],
    source.uvs[base + 2]
  };

  if (in_count == 2)
  {
    // Two vertices inside - create two triangles
    float t1, t2;
    mathz::Vec3 new_v1 = linePlaneIntersect(plane_point, plane_normal, in_vertices[0], out_vertices[0], t1);
    mathz::Vec3 new_v2 = linePlaneIntersect(plane_point, plane_normal, in_vertices[1], out_vertices[0], t2);

    // Interpolate UVs at clip points
    mathz::Vec2<float> new_uv1 = uvs[in_indices[0]] * (1.f - t1) + uvs[out_indices[0]] * t1;
    mathz::Vec2<float> new_uv2 = uvs[in_indices[1]] * (1.f - t2) + uvs[out_indices[0]] * t2;

    // Triangle 1: in[0], in[1], new_v1
    m_clipped_mesh.vertices.push_back(in_vertices[0]);
    m_clipped_mesh.vertices.push_back(in_vertices[1]);
    m_clipped_mesh.vertices.push_back(new_v1);

    m_clipped_mesh.normals.push_back(normal);
    m_clipped_mesh.normals.push_back(normal);
    m_clipped_mesh.normals.push_back(normal);

    m_clipped_mesh.uvs.push_back(uvs[in_indices[0]]);
    m_clipped_mesh.uvs.push_back(uvs[in_indices[1]]);
    m_clipped_mesh.uvs.push_back(new_uv1);

    m_clipped_mesh.z.push_back(0.f);
    m_clipped_mesh.z.push_back(0.f);
    m_clipped_mesh.z.push_back(0.f);

    m_clipped_mesh.materials.push_back(material);

    // Triangle 2: in[1], new_v2, new_v1
    m_clipped_mesh.vertices.push_back(in_vertices[1]);
    m_clipped_mesh.vertices.push_back(new_v2);
    m_clipped_mesh.vertices.push_back(new_v1);

    m_clipped_mesh.normals.push_back(normal);
    m_clipped_mesh.normals.push_back(normal);
    m_clipped_mesh.normals.push_back(normal);

    m_clipped_mesh.uvs.push_back(uvs[in_indices[1]]);
    m_clipped_mesh.uvs.push_back(new_uv2);
    m_clipped_mesh.uvs.push_back(new_uv1);

    m_clipped_mesh.z.push_back(0.f);
    m_clipped_mesh.z.push_back(0.f);
    m_clipped_mesh.z.push_back(0.f);

    m_clipped_mesh.materials.push_back(material);

    return true;
  }
  else if (in_count == 1)
  {
    // One vertex inside - create one triangle
    float t1, t2;
    mathz::Vec3 new_v1 = linePlaneIntersect(plane_point, plane_normal, in_vertices[0], out_vertices[0], t1);
    mathz::Vec3 new_v2 = linePlaneIntersect(plane_point, plane_normal, in_vertices[0], out_vertices[1], t2);

    mathz::Vec2<float> new_uv1 = uvs[in_indices[0]] * (1.f - t1) + uvs[out_indices[0]] * t1;
    mathz::Vec2<float> new_uv2 = uvs[in_indices[0]] * (1.f - t2) + uvs[out_indices[1]] * t2;

    m_clipped_mesh.vertices.push_back(in_vertices[0]);
    m_clipped_mesh.vertices.push_back(new_v1);
    m_clipped_mesh.vertices.push_back(new_v2);

    m_clipped_mesh.normals.push_back(normal);
    m_clipped_mesh.normals.push_back(normal);
    m_clipped_mesh.normals.push_back(normal);

    m_clipped_mesh.uvs.push_back(uvs[in_indices[0]]);
    m_clipped_mesh.uvs.push_back(new_uv1);
    m_clipped_mesh.uvs.push_back(new_uv2);

    m_clipped_mesh.z.push_back(0.f);
    m_clipped_mesh.z.push_back(0.f);
    m_clipped_mesh.z.push_back(0.f);

    m_clipped_mesh.materials.push_back(material);

    return true;
  }

  // in_count == 0 - triangle is fully clipped
  return true;
}

void Renderer::binTriangles()
{
  // Clear all tile triangle lists
  for (Tile& tile : m_tiles)
  {
    tile.triangles.clear();
    tile.clipped_triangles.clear();
  }

  // Bin main mesh triangles
  for (size_t i = 0; i < m_visible.size(); ++i)
  {
    if (!m_visible[i])
      continue;

    size_t base = i * 3;

    // Convert to screen space and compute bounding box
    auto [sx0, sy0] = imageToScreenSpace(m_render_mesh.vertices[base].x, m_render_mesh.vertices[base].y);
    auto [sx1, sy1] = imageToScreenSpace(m_render_mesh.vertices[base + 1].x, m_render_mesh.vertices[base + 1].y);
    auto [sx2, sy2] = imageToScreenSpace(m_render_mesh.vertices[base + 2].x, m_render_mesh.vertices[base + 2].y);

    int min_x = std::min({sx0, sx1, sx2});
    int max_x = std::max({sx0, sx1, sx2});
    int min_y = std::min({sy0, sy1, sy2});
    int max_y = std::max({sy0, sy1, sy2});

    // Clamp to screen
    min_x = std::max(min_x, 0);
    min_y = std::max(min_y, 0);
    max_x = std::min(max_x, (int)m_screen_width - 1);
    max_y = std::min(max_y, (int)m_screen_height - 1);

    if (min_x > max_x || min_y > max_y)
      continue;

    // Find overlapping tiles
    int tile_x0 = min_x / TILE_SIZE;
    int tile_y0 = min_y / TILE_SIZE;
    int tile_x1 = max_x / TILE_SIZE;
    int tile_y1 = max_y / TILE_SIZE;

    for (int ty = tile_y0; ty <= tile_y1; ++ty)
    {
      for (int tx = tile_x0; tx <= tile_x1; ++tx)
      {
        m_tiles[ty * m_tiles_x + tx].triangles.push_back(i);
      }
    }
  }

  // Bin clipped mesh triangles
  for (size_t i = 0; i < m_clipped_mesh.triangleCount(); ++i)
  {
    size_t base = i * 3;

    // Skip culled triangles
    if (m_clipped_mesh.z[base] < -100.f)
      continue;

    auto [sx0, sy0] = imageToScreenSpace(m_clipped_mesh.vertices[base].x, m_clipped_mesh.vertices[base].y);
    auto [sx1, sy1] = imageToScreenSpace(m_clipped_mesh.vertices[base + 1].x, m_clipped_mesh.vertices[base + 1].y);
    auto [sx2, sy2] = imageToScreenSpace(m_clipped_mesh.vertices[base + 2].x, m_clipped_mesh.vertices[base + 2].y);

    int min_x = std::min({sx0, sx1, sx2});
    int max_x = std::max({sx0, sx1, sx2});
    int min_y = std::min({sy0, sy1, sy2});
    int max_y = std::max({sy0, sy1, sy2});

    min_x = std::max(min_x, 0);
    min_y = std::max(min_y, 0);
    max_x = std::min(max_x, (int)m_screen_width - 1);
    max_y = std::min(max_y, (int)m_screen_height - 1);

    if (min_x > max_x || min_y > max_y)
      continue;

    int tile_x0 = min_x / TILE_SIZE;
    int tile_y0 = min_y / TILE_SIZE;
    int tile_x1 = max_x / TILE_SIZE;
    int tile_y1 = max_y / TILE_SIZE;

    for (int ty = tile_y0; ty <= tile_y1; ++ty)
    {
      for (int tx = tile_x0; tx <= tile_x1; ++tx)
      {
        m_tiles[ty * m_tiles_x + tx].clipped_triangles.push_back(i);
      }
    }
  }
}

void Renderer::renderTile(size_t tile_idx, const MeshData& model_mesh, const std::vector<Texture>& textures)
{
  const Tile& tile = m_tiles[tile_idx];
  int count = 0;

  for (size_t tri_idx : tile.triangles)
  {
    rasterize(tri_idx, m_render_mesh, model_mesh, textures, tile.x0, tile.y0, tile.x1, tile.y1);
    ++count;
  }

  for (size_t tri_idx : tile.clipped_triangles)
  {
    rasterize(tri_idx, m_clipped_mesh, m_clipped_mesh, textures, tile.x0, tile.y0, tile.x1, tile.y1);
    ++count;
  }

  m_triangles_rendered += count;
}

void Renderer::rasterize(size_t tri_idx, const MeshData& transformed, const MeshData& source, const std::vector<Texture>& textures, int tile_x0, int tile_y0, int tile_x1, int tile_y1)
{
  size_t base = tri_idx * 3;

  mathz::Vec2<int> v0 = {imageToScreenSpace(transformed.vertices[base].x, transformed.vertices[base].y)};
  mathz::Vec2<int> v1 = {imageToScreenSpace(transformed.vertices[base + 1].x, transformed.vertices[base + 1].y)};
  mathz::Vec2<int> v2 = {imageToScreenSpace(transformed.vertices[base + 2].x, transformed.vertices[base + 2].y)};

  // Compute triangle bounding box, clipped to tile bounds
  int min_x = std::max({std::min({v0.x, v1.x, v2.x}), tile_x0, 0});
  int max_x = std::min({std::max({v0.x, v1.x, v2.x}), tile_x1, (int)m_screen_width});
  int min_y = std::max({std::min({v0.y, v1.y, v2.y}), tile_y0, 0});
  int max_y = std::min({std::max({v0.y, v1.y, v2.y}), tile_y1, (int)m_screen_height});

  if (min_x >= max_x || min_y >= max_y)
    return;

  const Texture& texture = textures[source.materials[tri_idx]];

  const mathz::Vec3& n0 = transformed.normals[base];
  const mathz::Vec3& n1 = transformed.normals[base + 1];
  const mathz::Vec3& n2 = transformed.normals[base + 2];

  const mathz::Vec2<float>& uv0 = source.uvs[base];
  const mathz::Vec2<float>& uv1 = source.uvs[base + 1];
  const mathz::Vec2<float>& uv2 = source.uvs[base + 2];

  float tz0 = transformed.z[base];
  float tz1 = transformed.z[base + 1];
  float tz2 = transformed.z[base + 2];

  // Perspective-correct interpolation: recover 1/w from NDC z
  float pc_A = (m_far + m_near) / (m_far - m_near);
  float pc_B = 2.f * m_far * m_near / (m_far - m_near);
  float inv_w0 = (tz0 + pc_A) / pc_B;
  float inv_w1 = (tz1 + pc_A) / pc_B;
  float inv_w2 = (tz2 + pc_A) / pc_B;
  float u0w = uv0.x * inv_w0, v0w = uv0.y * inv_w0;
  float u1w = uv1.x * inv_w1, v1w = uv1.y * inv_w1;
  float u2w = uv2.x * inv_w2, v2w = uv2.y * inv_w2;

  float diffuse = -n0.dot(m_view_light);  // normals are inward, negate for outward
  if (diffuse < 0.f) diffuse = 0.f;
  float light = m_ambient + diffuse * m_diffuse_constant;
  if (light > 1.f) light = 1.f;

  float fx0 = (float)v0.x, fy0 = (float)v0.y;
  float fx1 = (float)v1.x, fy1 = (float)v1.y;
  float fx2 = (float)v2.x, fy2 = (float)v2.y;

  float e0_dx = fy1 - fy0;
  float e0_dy = fx0 - fx1;
  float e1_dx = fy2 - fy1;
  float e1_dy = fx1 - fx2;
  float e2_dx = fy0 - fy2;
  float e2_dy = fx2 - fx0;

  float area_t = (fx2 - fx0) * (fy1 - fy0) - (fy2 - fy0) * (fx1 - fx0);
  if (area_t == 0.0f) return;
  float inv_area = 1.0f / area_t;

  float start_x = min_x + 0.5f;
  float start_y = min_y + 0.5f;

  float e0_row = (start_x - fx0) * (fy1 - fy0) - (start_y - fy0) * (fx1 - fx0);
  float e1_row = (start_x - fx1) * (fy2 - fy1) - (start_y - fy1) * (fx2 - fx1);
  float e2_row = (start_x - fx2) * (fy0 - fy2) - (start_y - fy2) * (fx0 - fx2);

  __m128 e0_dx4 = _mm_set1_ps(e0_dx * 4.0f);
  __m128 e1_dx4 = _mm_set1_ps(e1_dx * 4.0f);
  __m128 e2_dx4 = _mm_set1_ps(e2_dx * 4.0f);
  __m128 e0_dx_step = _mm_setr_ps(0, e0_dx, e0_dx * 2, e0_dx * 3);
  __m128 e1_dx_step = _mm_setr_ps(0, e1_dx, e1_dx * 2, e1_dx * 3);
  __m128 e2_dx_step = _mm_setr_ps(0, e2_dx, e2_dx * 2, e2_dx * 3);
  __m128 zeros = _mm_setzero_ps();

  for (int y = min_y; y < max_y; y++)
  {
    float e0 = e0_row;
    float e1 = e1_row;
    float e2 = e2_row;

    int x = min_x;

    for (; x + 4 <= max_x; x += 4)
    {
      __m128 edge0 = _mm_add_ps(_mm_set1_ps(e0), e0_dx_step);
      __m128 edge1 = _mm_add_ps(_mm_set1_ps(e1), e1_dx_step);
      __m128 edge2 = _mm_add_ps(_mm_set1_ps(e2), e2_dx_step);

      __m128 mask0 = _mm_cmpge_ps(edge0, zeros);
      __m128 mask1 = _mm_cmpge_ps(edge1, zeros);
      __m128 mask2 = _mm_cmpge_ps(edge2, zeros);
      __m128 inside = _mm_and_ps(_mm_and_ps(mask0, mask1), mask2);

      int mask = _mm_movemask_ps(inside);

      if (mask != 0)
      {
        alignas(16) float e0_arr[4], e1_arr[4], e2_arr[4];
        _mm_store_ps(e0_arr, edge0);
        _mm_store_ps(e1_arr, edge1);
        _mm_store_ps(e2_arr, edge2);

        for (int i = 0; i < 4; ++i)
        {
          if (mask & (1 << i))
          {
            float w0 = e1_arr[i] * inv_area;
            float w1 = e2_arr[i] * inv_area;
            float w2 = e0_arr[i] * inv_area;

            float int_z = w0 * tz0 + w1 * tz1 + w2 * tz2;

            int px = x + i;
            int index = y * m_screen_width + px;

            if (int_z > m_depth_buffer[index])
            {
              float pc_w = w0 * inv_w0 + w1 * inv_w1 + w2 * inv_w2;
              float pc_u = (w0 * u0w + w1 * u1w + w2 * u2w) / pc_w;
              float pc_v = (w0 * v0w + w1 * v1w + w2 * v2w) / pc_w;
              Colour colour = texture.sample(pc_u, pc_v);

              Pixel pixel = {
                std::uint8_t(colour.r * light * 255),
                std::uint8_t(colour.g * light * 255),
                std::uint8_t(colour.b * light * 255),
                std::uint8_t(colour.a * 255)
              };

              int fb_index = index * 4;
              std::memcpy(&m_frame_buffer.get()[fb_index], &pixel, sizeof(Pixel));
              m_depth_buffer[index] = int_z;
            }
          }
        }
      }

      e0 += e0_dx * 4;
      e1 += e1_dx * 4;
      e2 += e2_dx * 4;
    }

    for (; x < max_x; x++)
    {
      if (e0 >= 0 && e1 >= 0 && e2 >= 0)
      {
        float w0 = e1 * inv_area;
        float w1 = e2 * inv_area;
        float w2 = e0 * inv_area;

        float int_z = w0 * tz0 + w1 * tz1 + w2 * tz2;

        int index = y * m_screen_width + x;

        if (int_z > m_depth_buffer[index])
        {
          float pc_w = w0 * inv_w0 + w1 * inv_w1 + w2 * inv_w2;
          float pc_u = (w0 * u0w + w1 * u1w + w2 * u2w) / pc_w;
          float pc_v = (w0 * v0w + w1 * v1w + w2 * v2w) / pc_w;
          Colour colour = texture.sample(pc_u, pc_v);

          Pixel pixel = {
            std::uint8_t(colour.r * light * 255),
            std::uint8_t(colour.g * light * 255),
            std::uint8_t(colour.b * light * 255),
            std::uint8_t(colour.a * 255)
          };

          int fb_index = index * 4;
          std::memcpy(&m_frame_buffer.get()[fb_index], &pixel, sizeof(Pixel));
          m_depth_buffer[index] = int_z;
        }
      }

      e0 += e0_dx;
      e1 += e1_dx;
      e2 += e2_dx;
    }

    e0_row += e0_dy;
    e1_row += e1_dy;
    e2_row += e2_dy;
  }
}
