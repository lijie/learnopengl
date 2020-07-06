#ifndef __LEARNOPENGL_COMMON_CUBEMAP_H__
#define __LEARNOPENGL_COMMON_CUBEMAP_H__

#include <memory>
#include <string>
#include <vector>

#include "renderer.h"

class Material;

class CubeMap : public Renderer {
 public:
  CubeMap() {}
  CubeMap(const std::vector<std::string> &path);
  int GetTextureId();

  void Render(GlContext *c) override;

  void set_material(std::shared_ptr<Material> m) { material_ = m; }

 private:
  int texture_id_;
  unsigned int vao, vbo, ebo;
  std::shared_ptr<Material> material_ = nullptr;

  void Submit();
};

#endif  // __LEARNOPENGL_COMMON_CUBEMAP_H__
