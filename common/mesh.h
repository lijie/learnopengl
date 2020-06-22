#ifndef __LEARNOPENGL_COMMON_MESH_H__
#define __LEARNOPENGL_COMMON_MESH_H__

#include <vector>

#include "lo_common.h"
#include "material.h"

struct Vertex {
  Vec3 Position;
  Vec3 Normal;
  Vec2 TexCoord;
  Vec3 Tangent;
  Vec3 Bitangent;
};

class Mesh {
 public:
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  // std::vector<Texture> textures;
  std::shared_ptr<Material> material;

  private:
  int vao, vbo, ebo;

  void setupMesh();
};

#endif  // __LEARNOPENGL_COMMON_MESH_H__
