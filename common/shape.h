#ifndef __LEARNOPENGL_COMMON_SHAPE_H__
#define __LEARNOPENGL_COMMON_SHAPE_H__

#include <functional>
#include <memory>

#include "lo_common.h"
#include "renderer.h"
#include "transform.h"

class Material;
class GlContext;

class Shape : public Renderer, public Transform {
 public:
  Shape();
  Shape(shared_ptr<Material> mat) { material_ = mat; }
  ~Shape();
  float *vertices() { return vertices_; }
  int vertex_size() { return vertex_size_; }
  void set_vertices(float *v, int n) {
    vertices_ = v;
    vertex_size_ = n;
  }

  void set_material(std::shared_ptr<Material> m) { material_ = m; }

  void Translate(const Vec3 &v);
  void Scale(const Vec3 &v);

  // Vec3 position() { return position_; }
  // Vec3 scale() { return scale_; }

  void set_color(const Vec3 &v) { albedo_ = v; }
  Vec3 color() { return albedo_; }

  void set_stencil_mask(uint8_t mask) { stencil_mask_ = mask; }
  void set_render_callback(std::function<void(void)> before,
                           std::function<void(void)> after) {
    before_render_func_ = before;
    after_render_func_ = after;
  }

  void Render(GlContext *ctx) override;

  virtual void Submit();

 protected:
  float *vertices_ = nullptr;
  int vertex_size_ = 0;
  // Vec3 position_;
  // Vec3 scale_;
  Vec3 albedo_;
  unsigned int vao, vbo, ebo;
  uint8_t stencil_mask_ = 0x00;
  // Mat4 model_;
  std::shared_ptr<Material> material_ = nullptr;

  std::function<void(void)> before_render_func_ = nullptr;
  std::function<void(void)> after_render_func_ = nullptr;
};

class Cube : public Shape {
 public:
  Cube();
  void Submit() override;
};

class Plane : public Shape {
 public:
  Plane();
  void Submit() override;
};

class Quad : public Shape {
 public:
  Quad();
  void Submit() override;
};

class LightSource : public Cube {
 public:
  LightSource();

  void set_power(float v) { power_ = v; };
  float power() { return power_; }

 protected:
  float power_ = 40.0;
};

#endif  // __LEARNOPENGL_COMMON_SHAPE_H__
