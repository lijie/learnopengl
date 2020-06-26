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

class Mesh : public Renderer, public Transform {
 public:
  Mesh();
  ~Mesh();
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  // std::vector<Texture> textures;
  std::shared_ptr<Material> material;

  void Render(GlContext *ctx) override;

 private:
  unsigned int vao, vbo, ebo;
  bool submit_done_ = false;

  void Submit();
};

#endif  // __LEARNOPENGL_COMMON_MESH_H__
