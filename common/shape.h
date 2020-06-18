#ifndef __LEARNOPENGL_COMMON_H__
#define __LEARNOPENGL_COMMON_H__

#include <memory>

class Material;

class Shape {};

class Cube : public Shape {
 public:
  float *vertices() { return vertices_; }
  int vertex_size() { return vertex_size_; }
  void set_vertices(float *v, int n) {
    vertices_ = v;
    vertex_size_ = n;
  }

  void set_material(std::shared_ptr<Material> m) { material_ = m; }

 private:
  float *vertices_ = nullptr;
  int vertex_size_ = 0;

  std::shared_ptr<Material> material_ = nullptr;
};

#endif  // __LEARNOPENGL_COMMON_H__