#ifndef __LEARNOPENGL_COMMON_MATERIAL_H__
#define __LEARNOPENGL_COMMON_MATERIAL_H__

#include <any>
#include <map>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

#include "lo_common.h"
#include "shader.h"

class Shader;
class Texture;
class GlContext;
class Transform;

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
  Texture() {}
  ~Texture();
  // opengl texutre id
  int texture_id = -1;
  TextureType type = kInvalidTexture;
  int channel = 0;
  int width = 0;
  int height = 0;
  uint8_t* data = nullptr;
  bool is_cube_map = false;

  void SetupTexture();
  void Unbind();

  static std::shared_ptr<Texture> NewTexture(const std::string&,
                                             TextureType type);
  static std::shared_ptr<Texture> NewTextureWithTextureId(int tex_id);
};

typedef std::shared_ptr<Texture> texture_t;

#define MATERIAL_TEXTURE_NUM 16
#define MATERIAL_COLOR_NUM 16
class Material {
 public:
  Material(){};
  Material(const std::string& shader_path);
  std::shared_ptr<Shader> shader;
  std::vector<texture_t> textures;

  template <typename T>
  void SetProperty(std::string& name, T value) {
    std::any holder = value;
    properties_[name] = holder;
  }

  // deprecated
  int texture_id[MATERIAL_TEXTURE_NUM];

  void UseShader(Transform* t);

  void SetProperty(const std::string& name, const std::any& value) {
    properties_[name] = value;
  }

 private:
  void UpdateShaderUniforms(Transform* t);
  std::map<std::string, std::any> properties_;
  std::unordered_map<std::string, int> valid_properties_;

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
