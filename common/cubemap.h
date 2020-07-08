#ifndef __LEARNOPENGL_COMMON_CUBEMAP_H__
#define __LEARNOPENGL_COMMON_CUBEMAP_H__

#include <memory>
#include <string>
#include <vector>

#include "shape.h"

class Material;

class CubeMap : public Shape {
 public:
  CubeMap() {}
  CubeMap(const std::vector<std::string> &path);

  // void Render() override;
  void Submit() override;

 private:
  int texture_id_;
  unsigned int vao, vbo, ebo;  
};

#endif  // __LEARNOPENGL_COMMON_CUBEMAP_H__
