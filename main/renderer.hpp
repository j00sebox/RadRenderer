#include "camera.hpp"
#include "model.hpp"

#include <cstdint>
#include <memory>

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

private:
  void rasterize(const Triangle& t, const Texture& texture = {});
  float edgeFunction(float x0, float y0, float x1, float y1, float x2, float y2);
  void setPixel(int x, int y, const Pixel& col);
  Pixel getColour(float lum);
  std::pair<int, int> imageToScreenSpace(float x, float y);
  mathz::Vec3 linePlaneIntersect(const mathz::Vec3& point, const mathz::Vec3& plane_normal, mathz::Vec3& line_begin, mathz::Vec3& line_end);
  bool clipTriangle(const mathz::Vec3& plane_point, const mathz::Vec3& plane_normal, Triangle& t);
  inline void transformTriangle(Triangle& t, const mathz::Mat4& transform);
  inline mathz::Vec3 calculateNormal(Triangle& t);
  void clearFrameBuffer();

  float m_far, m_near;

  mathz::Mat4 m_view;
  std::vector<Triangle> m_render_triangles;
  std::vector<Triangle> m_clipped_triangles;

  // Lighting stuff
  mathz::Vec3 m_directional_light;
  float m_diffuse_constant = 0.75f;

  // Screen stuff
  unsigned int m_screen_width, m_screen_height, m_buffer_size;
  std::unique_ptr<std::uint8_t> m_frame_buffer;
  std::vector<float> m_depth_buffer;
};
