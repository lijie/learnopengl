#ifndef __LEARNOPENGL_RENDERER_H__
#define __LEARNOPENGL_RENDERER_H__

#include <memory>
#include "transform.h"

class GlContext;
class Material;

class Renderer : public Transform {
 public:
  // submit data to GPU
  // .. set vao, vbo, ebo and so on...
  // .. set shader uniform values
  virtual void Submit() = 0;

  // do render
  // glDrawXXXX
  virtual void Render() = 0;

  // after render, clean data if necessary
  virtual void Cleanup() {}

  // update per frame
  void Update(float dt) {
    Submit();
    Render();
    Cleanup();
  }

  void set_proority(int v) { priority_ = v; }
  int priority() const { return priority_; }

  void set_material(std::shared_ptr<Material> m) { material_ = m; }
  std::shared_ptr<Material> material() { return material_; }

 protected:
  int priority_ = 0;
  std::shared_ptr<Material> material_ = nullptr;
};

#endif  // __LEARNOPENGL_RENDERER_H__
