#ifndef __LEARNOPENGL_COMMON_SHAPE_H__
#define __LEARNOPENGL_COMMON_SHAPE_H__

#include <memory>

#include "lo_common.h"
#include "renderer.h"

class Material;
class GlContext;

class Shape : public Renderer {
 public:
  Shape();
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

  Vec3 position() { return position_; }
  Vec3 scale() { return scale_; }

  void set_color(const Vec3 &v) { albedo_ = v; }
  Vec3 color() { return albedo_; }

  void Render(GlContext *ctx) override;

  virtual void Submit() = 0;

 protected:
  float *vertices_ = nullptr;
  int vertex_size_ = 0;
  Vec3 position_;
  Vec3 scale_;
  Vec3 albedo_;
  unsigned int vao, vbo, ebo;
  // Mat4 model_;
  std::shared_ptr<Material> material_ = nullptr;
};

class Cube : public Shape {
 public:
  Cube();
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
