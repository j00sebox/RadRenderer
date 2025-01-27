#include "camera.hpp"
#include "model.hpp"

#include <cstdint>
#include <memory>

class Renderer
{
public:
  Renderer(unsigned int screen_width, unsigned int screen_height, float near, float far);
  ~Renderer() {}

  void Render(const Model& model, const Camera& camera);
  std::uint8_t* GetFrameBuffer() const { return m_frame_buffer.get(); }

private:
  void Rasterize(const Triangle& t);
  float EdgeFunction(float x0, float y0, float x1, float y1, float x2, float y2);
  void SetPixel(int x, int y, const Pixel& col);
  std::pair<int, int> ImageToScreenSpace(float x, float y);
  void ClearFrameBuffer();
  mathz::Vec3 LinePlaneIntersect(mathz::Vec3& point, mathz::Vec3& plane_normal, mathz::Vec3& line_begin, mathz::Vec3& line_end);
  bool ClipTriangle(mathz::Vec3&& point, mathz::Vec3&& plane_normal, Triangle& t);
  bool OutNearFarBounds(const Triangle& t);
  inline void TransformTriangle(Triangle& t, const mathz::Mat4& transform);
  inline mathz::Vec3 CalculateNormal(Triangle& t);

  float m_far, m_near;

  mathz::Mat4 m_view;
  std::vector<Triangle> m_render_triangles;
  std::vector<Triangle> m_clipped_triangles;

  // Screen stuff
  unsigned int m_screen_width, m_screen_height, m_buffer_size;
  std::unique_ptr<std::uint8_t> m_frame_buffer;
};
