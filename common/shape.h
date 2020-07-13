#ifndef __LEARNOPENGL_COMMON_SHAPE_H__
#define __LEARNOPENGL_COMMON_SHAPE_H__

#include <functional>
#include <memory>

#include "lo_common.h"
#include "renderer.h"
#include "transform.h"

class Material;
class GlContext;

struct VAOAttr {
  int size;
  int type;
  int normalize;
  size_t stride;
  size_t offset;

  int external_vbo;
  int divisor;

  VAOAttr() {}

  VAOAttr(int size_, int type_, int normalize_, size_t stride_, size_t offset_) {
    Reset(size_, type_, normalize_, stride_, offset_);
  }

  void Reset(int size_, int type_, int normalize_, size_t stride_, size_t offset_) {
    size = size_;
    type = type_;
    normalize = normalize_;
    stride = stride_;
    offset = offset_;

    external_vbo = -1;
    divisor = 0;
  }
};

class Shape : public Renderer {
 public:
  Shape();
  Shape(shared_ptr<Material> mat) {
    set_material(mat);
  }
  ~Shape();
  float *vertices() { return vertices_; }
  int vertex_size() { return vertex_size_; }
  void set_vertices(float *v, int n) {
    vertices_ = v;
    vertex_size_ = n;
  }

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

  void append_vao_attr(const VAOAttr& attr) {
    vao_attr_vec_.push_back(attr);
  }

  size_t vao_attr_size() {
    return vao_attr_vec_.size();
  }

  void enable_instance(bool flag, unsigned int num) {
    enable_instance_ = flag;
    instance_num_ = num;
  }

  void Render() override;
  void Submit() override;
  void Cleanup() override;

  int vbo() {return vbo_;}
  int vao() {return vao_;}
  int ebo() {return ebo_;}

 protected:
  float *vertices_ = nullptr;
  int vertex_size_ = 0;
  // Vec3 position_;
  // Vec3 scale_;
  Vec3 albedo_;
  unsigned int vao_, vbo_, ebo_;
  uint8_t stencil_mask_ = 0x00;
  bool enable_instance_ = false;
  unsigned int instance_num_ = 0;
  // Mat4 model_;

  std::vector<VAOAttr> vao_attr_vec_;

  std::function<void(void)> before_render_func_ = nullptr;
  std::function<void(void)> after_render_func_ = nullptr;
};

class Cube : public Shape {
 public:
  Cube();
};

class Plane : public Shape {
 public:
  Plane();
};

class Quad : public Shape {
 public:
  Quad();
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
