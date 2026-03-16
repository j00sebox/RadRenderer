#include "camera.hpp"
#include "model.hpp"

#include <cstdint>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>

extern Camera* g_camera;

struct RenderStats
{
  int triangles_rendered = 0;
};

struct Tile
{
  int x0, y0, x1, y1;  // Screen bounds
  std::vector<size_t> triangles;  // Indices into render mesh
  std::vector<size_t> clipped_triangles;  // Indices into clipped mesh
};

struct Pixel
{
  Pixel() : r(0), g(0), b(0), a(0) {}
  Pixel(std::uint8_t _r, std::uint8_t _g, std::uint8_t _b, std::uint8_t _a) : r(_r), g(_g), b(_b), a(_a) {}

  std::uint8_t r, g, b, a;
};

class Renderer
{
public:
  Renderer(unsigned int screen_width, unsigned int screen_height, float near, float far);
  ~Renderer();

  void render(const Model& model);
  std::uint8_t* getFrameBuffer() const { return m_frame_buffer.get(); }
  const RenderStats& getStats() const { return m_stats; }

private:
  void rasterize(size_t tri_idx, const MeshData& transformed, const MeshData& source, const std::vector<Texture>& textures, int tile_x0, int tile_y0, int tile_x1, int tile_y1);
  void rasterize(size_t tri_idx, const MeshData& transformed, const MeshData& source, const std::vector<Texture>& textures);
  float edgeFunction(float x0, float y0, float x1, float y1, float x2, float y2);
  void setPixel(int x, int y, const Pixel& col);
  Pixel getColour(float lum);
  std::pair<int, int> imageToScreenSpace(float x, float y);
  mathz::Vec3 linePlaneIntersect(const mathz::Vec3& point, const mathz::Vec3& plane_normal, mathz::Vec3& line_begin, mathz::Vec3& line_end, float& t);
  bool clipTriangle(const mathz::Vec3& plane_point, const mathz::Vec3& plane_normal, size_t tri_idx, MeshData& transformed, const MeshData& source);
  void transformVertices(MeshData& mesh, const mathz::Mat4& transform);
  void transformVertices(const std::vector<mathz::Vec3>& src, MeshData& dst, const mathz::Mat4& transform);
  mathz::Vec3 calculateNormal(size_t tri_idx, const MeshData& mesh);
  void clearFrameBuffer();
  void binTriangles();
  void renderTile(size_t tile_idx, const MeshData& model_mesh, const std::vector<Texture>& textures);

  float m_far, m_near;

  mathz::Mat4 m_view;
  MeshData m_render_mesh;    // Transformed vertices, normals, z
  MeshData m_clipped_mesh;   // SoA for clipped triangles
  std::vector<bool> m_visible; // Triangle visibility flags

  // Lighting stuff
  mathz::Vec3 m_directional_light;
  mathz::Vec3 m_view_light;  // m_directional_light transformed to view space each frame
  float m_diffuse_constant = 0.75f;
  float m_ambient = 0.4f;

  // Screen stuff
  unsigned int m_screen_width, m_screen_height, m_buffer_size;
  std::unique_ptr<std::uint8_t> m_frame_buffer;
  std::vector<float> m_depth_buffer;
  RenderStats m_stats;

  // Frustum
  mathz::Vec3 m_frustum_planes[4];

  // Tiling
  static constexpr int TILE_SIZE = 64;
  std::vector<Tile> m_tiles;
  unsigned int m_tiles_x, m_tiles_y;
  std::atomic<int> m_triangles_rendered{0};

  // Thread pool
  std::vector<std::thread> m_thread_pool;
  std::mutex m_pool_mutex;
  std::condition_variable m_pool_cv;
  std::condition_variable m_pool_done_cv;
  std::function<void()> m_pool_task;
  unsigned int m_pool_size;
  unsigned int m_pool_epoch = 0;
  std::atomic<unsigned int> m_pool_finished{0};
  bool m_pool_shutdown = false;
};
