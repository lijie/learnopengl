#ifndef __LEARNOPENGL_COMMON_H__
#define __LEARNOPENGL_COMMON_H__

#include <memory>

#include "lo_common.h"
#include "renderer.h"

class Material;
class GLContext;

class Shape : public Renderer {
 public:
  Shape();
  float *vertices() { return vertices_; }
  int vertex_size() { return vertex_size_; }
  void set_vertices(float *v, int n) {
    vertices_ = v;
    vertex_size_ = n;
  }

  void set_material(std::shared_ptr<Material> m) { material_ = m; }

  void Translate(const Vec3& v);
  void Scale(const Vec3& v);

  void Draw(GLContext *ctx) override;

 protected:
  float *vertices_ = nullptr;
  int vertex_size_ = 0;
  Mat4 model_;
  std::shared_ptr<Material> material_ = nullptr;
};

class Cube : public Shape {
  public:
    Cube();
};

#endif  // __LEARNOPENGL_COMMON_H__
