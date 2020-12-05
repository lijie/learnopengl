#ifndef __LEARNOPENGL_COMMON_MESH_H__
#define __LEARNOPENGL_COMMON_MESH_H__

#include <vector>

#include "lo_common.h"
#include "material.h"
#include "renderer.h"
#include "transform.h"

struct Vertex {
  Vec3 Position;
  Vec3 Normal;
  Vec2 TexCoord;
  Vec3 Tangent;
  Vec3 Bitangent;
};

class Mesh : public Renderer {
 public:
  Mesh();
  ~Mesh();
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  // std::vector<Texture> textures;
  // std::shared_ptr<Material> material;

  void Render(MaterialPtr mat) override;
  void Submit(MaterialPtr mat) override;

 private:
  unsigned int vao, vbo, ebo;
  bool submit_done_ = false;
};

#endif  // __LEARNOPENGL_COMMON_MESH_H__
