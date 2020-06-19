#ifndef __LEARNOPENGL_COMMON_MATERIAL_H__
#define __LEARNOPENGL_COMMON_MATERIAL_H__

#include <memory>
#include "lo_common.h"

class Shader;
class Texture;

// standard alias
enum ColorAlias {
  kAlbedo = 0,
};
enum TextureAlias {
  kMainTex = 0,
  kNormalMap = 1,

  kCustomTex1 = 4,
  kCustomTex2 = 5,
  kCustomTex3 = 6,
  kCustomTex4 = 7,
};

#define MATERIAL_TEXTURE_NUM  16
#define MATERIAL_COLOR_NUM  16
class Material {
 public:
  Material();
  std::shared_ptr<Shader> shader;
  std::shared_ptr<Texture> textures[16];
  Vec3 colors[16];

  void UpdateShaderUniforms();
};

Material::Material() {
  for(int i = 0; i < MATERIAL_TEXTURE_NUM; i++) {
    textures[i] = nullptr;
  }
  for(int i = 0; i < MATERIAL_COLOR_NUM; i++) {
    colors[i] = INVALID_COLOR;
  }
}

void Material::UpdateShaderUniforms() {

}

#endif  // __LEARNOPENGL_COMMON_MATERIAL_H__
