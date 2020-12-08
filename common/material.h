#ifndef __LEARNOPENGL_COMMON_MATERIAL_H__
#define __LEARNOPENGL_COMMON_MATERIAL_H__

#include <any>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
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
  static std::shared_ptr<Texture> NewColorTexture(const Vec3& color);
};

typedef std::shared_ptr<Texture> texture_t;

struct MaterialParams {
  float Shininess;
  Vec3 Albedo;
  Vec3 Specular;
  std::string DiffuseTexture;
  std::string SpecularTexture;
  TexturePtr DiffuseTexturePtr;
  TexturePtr SpecularTexturePtr;
};

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

  void UseShader();

  void SetProperty(const std::string& name, const std::any& value) {
    properties_[name] = value;
  }

  void DefineValue(const std::string& name);
  void DefineValue(const std::string& name, int value);

  // common uniforms setters
  void SetDiffuseTexture(texture_t tex);
  void SetNormalTexture(texture_t tex);
  void SetSpecularTexture(texture_t tex);

  bool HasProperty(const std::string& name) {
    return properties_.find(name) != properties_.end();
  }

  void SetParams(const MaterialParams& params);

 private:
  void UpdateShaderUniforms();
  std::map<std::string, std::any> properties_;
  std::unordered_map<std::string, int> valid_properties_;

  // static void Init();
  // static material_t FindMaterial(const std::string& name);
  // static shader_t LoadShader(const std::string& name);
};

typedef std::shared_ptr<Material> material_t;

class PhongMaterial : public Material {
 public:
  PhongMaterial() : Material("../shaders/phong") {}
};

class DepthMapMaterial : public Material {
  public:
  DepthMapMaterial() : Material("../shaders/depthmap") {}
};

class UnlitMaterial : public Material {
  public:
  UnlitMaterial() : Material("../shaders/unlit") {}
};

class ToonMaterial : public Material {
  public:
  ToonMaterial() : Material("../shaders/toon") {}
};

// common uniform name
#define DIFFUSE_TEXTURE "uDiffuseTexture"
#define DIFFUSE_MACRO "USE_DIFFUSE_TEXTURE"

#define NORMAL_TEXTURE "uNormalTexture"
#define NORMAL_MACRO "USE_NORMAL_TEXTURE"

#define SPECULAR_TEXTURE "uSpecularTexture"
#define SPECULAR_MACRO "USE_SPECULAR_TEXTURE"

#endif  // __LEARNOPENGL_COMMON_MATERIAL_H__
