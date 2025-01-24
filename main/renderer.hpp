#include "camera.hpp"
#include "model.hpp"

#include <memory>

struct RendererSettings
{
  float near, far, fov;
};

class Renderer
{
public:
  Renderer(unsigned int screen_width, unsigned int screen_height, float near, float far);
  ~Renderer() {}

  Pixel* Render(const Model& model, const Camera& camera, float elapsed_time, float cam_forward, float rotate_x, float rotate_y);

private:
  void Rasterize(const Triangle& t);
  float EdgeFunction(float x0, float y0, float x1, float y1, float x2, float y2);
  Pixel GetColour(float lum);
  void SetPixel(int x, int y, const Pixel& col);
  std::pair<int, int> ImageToScreenSpace(float x, float y);
  void ClearFrameBuffer();
  void ClearDepthBuffer();
  mathz::Vec3 LinePlaneIntersect(mathz::Vec3& point, mathz::Vec3& plane_normal, mathz::Vec3& line_begin, mathz::Vec3& line_end);
  bool ClipTriangle(mathz::Vec3&& point, mathz::Vec3&& plane_normal, Triangle& t);
  bool OutNearFarBounds(const Triangle& t);
  inline void TransformTriangle(Triangle& t, const mathz::Mat4& transform);
  inline mathz::Vec3 CalculateNormal(Triangle& t);

  float m_far, m_near;

  // Transforms
  mathz::Mat4 m_view;

  // Lighting
  mathz::Vec3 m_directional_light;
  float m_diffuse_constant = 0.75f; // make some kind of material class for this

  std::vector<Triangle> m_render_triangles;
  std::vector<Triangle> m_clipped_tris;

  // Screen stuff
  unsigned int m_screen_width, m_screen_height, m_buffer_size;
  int m_half_width, m_half_height; // caching these for later
  std::unique_ptr<Pixel> m_frame_buffer;
  std::vector<float> m_depth_buffer;
};
