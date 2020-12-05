#ifndef __LEARNOPENGL_COMMON_MODEL_H__
#define __LEARNOPENGL_COMMON_MODEL_H__

#include <stdio.h>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "material.h"
#include "mesh.h"

class GlContext;

class Model : public Renderer {
 public:
  Model() {
      visible_ = false;
  }
  bool Load(const std::string &path, const std::string& texture_location,
      bool gamma = false, bool flip_uv = true);
  // std::vector<Mesh *> meshes;

  void Render(MaterialPtr mat) override;
  void Submit(MaterialPtr mat) override {}

 private:
  std::string directory_;
  bool enable_texture_location_ = false;

  void ProcessNode(aiNode *node, const aiScene *scene);
  std::shared_ptr<Mesh> ProcessMesh(aiMesh *mesh, const aiScene *scene);

  texture_t LoadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                 TextureType type_alias);
};


#endif  // #define __LEARNOPENGL_COMMON_MODEL_H__
