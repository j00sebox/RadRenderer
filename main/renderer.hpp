#include "camera.hpp"
#include "model.hpp"

#include <cstdint>
#include <memory>

struct RenderStats
{
  int triangles_rendered = 0;
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
  ~Renderer() {}

  void render(const Model& model, Camera& camera);
  std::uint8_t* getFrameBuffer() const { return m_frame_buffer.get(); }
  const RenderStats& getStats() const { return m_stats; }

private:
  void rasterize(size_t tri_idx, const MeshData& transformed, const MeshData& source, const std::vector<Texture>& textures);
  float edgeFunction(float x0, float y0, float x1, float y1, float x2, float y2);
  void setPixel(int x, int y, const Pixel& col);
  Pixel getColour(float lum);
  std::pair<int, int> imageToScreenSpace(float x, float y);
  mathz::Vec3 linePlaneIntersect(const mathz::Vec3& point, const mathz::Vec3& plane_normal, mathz::Vec3& line_begin, mathz::Vec3& line_end);
  bool clipTriangle(const mathz::Vec3& plane_point, const mathz::Vec3& plane_normal, size_t tri_idx, MeshData& transformed, const MeshData& source);
  void transformVertices(MeshData& mesh, const mathz::Mat4& transform);
  mathz::Vec3 calculateNormal(size_t tri_idx, const MeshData& mesh);
  void clearFrameBuffer();

  float m_far, m_near;

  mathz::Mat4 m_view;
  MeshData m_render_mesh;    // Transformed vertices, normals, z
  MeshData m_clipped_mesh;   // SoA for clipped triangles
  std::vector<bool> m_visible; // Triangle visibility flags

  // Lighting stuff
  mathz::Vec3 m_directional_light;
  float m_diffuse_constant = 0.75f;

  // Screen stuff
  unsigned int m_screen_width, m_screen_height, m_buffer_size;
  std::unique_ptr<std::uint8_t> m_frame_buffer;
  std::vector<float> m_depth_buffer;
  RenderStats m_stats;
};
