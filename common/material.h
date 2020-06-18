#ifndef __LEARNOPENGL_COMMON_MATERIAL_H__
#define __LEARNOPENGL_COMMON_MATERIAL_H__

#include <memory>

class Shader;

class Material {
 public:
  Material() {}
  std::shared_ptr<Shader> shader;
};

#endif  // __LEARNOPENGL_COMMON_MATERIAL_H__