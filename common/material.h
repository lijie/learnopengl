#ifndef __LEARNOPENGL_COMMON_MATERIAL_H__
#define __LEARNOPENGL_COMMON_MATERIAL_H__

#include <memory>
#include <string>
#include <vector>

#include "lo_common.h"
#include "shader.h"

class Shader;
class Texture;
class GlContext;

// standard alias
enum ColorAlias {
  kAlbedo = 0,
};

enum TextureType {
  kMainTex = 0,
  kDiffuseTex = 0,
  kSpecularTex = 1,
  kNormalTex = 2,
  kHeightTex = 3,

  kCustomTex1 = 4,
  kCustomTex2 = 5,
  kCustomTex3 = 6,
  kCustomTex4 = 7,

  kInvalidTexture,
};

class Texture {
 public:
  Texture() {
  }
  ~Texture();
  // opengl texutre id
  int texture_id = -1;
  TextureType type = kInvalidTexture;
  int channel = 0;
  int width = 0;
  int height = 0;
  uint8_t *data = nullptr;

  void SetupTexture();
  void Unbind();

  static std::shared_ptr<Texture> NewTexture(const std::string&, TextureType type);
};

typedef std::shared_ptr<Texture> texture_t;

#define MATERIAL_TEXTURE_NUM 16
#define MATERIAL_COLOR_NUM 16
class Material {
 public:
  Material(){};
  std::shared_ptr<Shader> shader;
  std::vector<texture_t> textures;
  float phong_exponent = 5;
  // std::vector<texture_t> diffuse_textures;
  // std::vector<texture_t> specular_textures;
  // std::vector<texture_t> normal_textures;
  // std::vector<texture_t> height_textures;
  Vec3 colors[MATERIAL_COLOR_NUM];

  // deprecated
  int texture_id[MATERIAL_TEXTURE_NUM];

  void UseShader(Mat4 model);

 private:
  void UpdateShaderUniforms(Mat4 model);

  // static void Init();
  // static material_t FindMaterial(const std::string& name);
  // static shader_t LoadShader(const std::string& name);
};

typedef std::shared_ptr<Material> material_t;

// Material::Material() {
// for(int i = 0; i < MATERIAL_TEXTURE_NUM; i++) {
//   textures[i] = nullptr;
// }
// for(int i = 0; i < MATERIAL_COLOR_NUM; i++) {
//   colors[i] = INVALID_COLOR;
// }
// }

// void Material::UpdateShaderUniforms() {

// }

#endif  // __LEARNOPENGL_COMMON_MATERIAL_H__
