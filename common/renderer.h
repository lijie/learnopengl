#ifndef __LEARNOPENGL_RENDERER_H__
#define __LEARNOPENGL_RENDERER_H__

#include <string>
#include <memory>
#include "transform.h"

class GlContext;
class Material;

class Renderer : public Transform {
 public:
  // submit data to GPU
  // .. set vao, vbo, ebo and so on...
  // .. set shader uniform values
  virtual void Submit(MaterialPtr mat) {};

  // do render
  // glDrawXXXX
  virtual void Render(MaterialPtr mat) {};

  // after render, clean data if necessary
  virtual void Cleanup() {}

  virtual void BeforeUpdate() {}
  virtual void AfterUpdate() {}

  // update per frame
  void Update(MaterialPtr material) {
    BeforeUpdate();
    Submit(material);
    Render(material);
    Cleanup();
    AfterUpdate();
  }

  void set_proority(int v) { priority_ = v; }
  int priority() const { return priority_; }
  bool visible() { return visible_; }

  // set self material
  void set_material(std::shared_ptr<Material> m) { material_ = m; }
  std::shared_ptr<Material> material() { return material_; }

  std::vector<std::shared_ptr<Renderer>>& GetChildren() { return children_; }

  void set_display_name(const std::string& name) {display_name_ = name;}

 protected:
  int priority_ = 0;
  bool visible_ = true;
  bool need_submit_ = true;
  std::shared_ptr<Material> material_ = nullptr;
  std::string display_name_ = "";

  // TODO(andrewli):
  // move to transform...
  std::vector<std::shared_ptr<Renderer>> children_;
};

#endif  // __LEARNOPENGL_RENDERER_H__
