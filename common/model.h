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
  Model() {}
  bool Load(const std::string &path, bool gamma = false);
  // std::vector<Mesh *> meshes;

  void Render() override;
  void Submit() override {}

 private:
  std::string directory_;

  void ProcessNode(aiNode *node, const aiScene *scene);
  std::shared_ptr<Mesh> ProcessMesh(aiMesh *mesh, const aiScene *scene);

  texture_t LoadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                 TextureType type_alias);
};


#endif  // #define __LEARNOPENGL_COMMON_MODEL_H__
