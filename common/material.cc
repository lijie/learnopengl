#include "material.h"

#include <map>
#include <string>

#include "context.h"
#include "glad/glad.h"
#include "lo_common.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static std::map<std::string, material_t> MaterialCollections;
static std::map<std::string, shader_t> ShaderCollections;

#if 0
void Material::Init() {
}

material_t Material::FindMaterial(const std::string& name) {
  auto it = MaterialCollections.find(name);
  if (it == MaterialCollections.end())
    return nullptr;
  return it.second();
}
#endif

static std::map<std::string, texture_t> TextureCollections;

Texture::~Texture() {
  glDeleteTextures(1, (const GLuint*)&texture_id);
}

void Texture::SetupTexture() {
  // check texture id
  // int idx = texture_id - GL_TEXTURE0;
  // if (idx >= GL_TEXTURE0 && idx < GL_TEXTURE0 + 16) return;

  unsigned int id;
  glGenTextures(1, &id);

  GLenum format = GL_RGB;
  if (channel == 1) {
    format = GL_RED;
  } else if (channel == 4) {
    format = GL_RGBA;
  }

  // TODO:
  // OpenGL 同时能绑定的tex是有上限的, 应该用时再绑定并传递数据?

  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  auto wrap = GL_REPEAT;
  if (format == GL_RGBA) wrap = GL_CLAMP_TO_EDGE;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  texture_id = id;
}

static texture_t GenerateColorTexture(const Vec3& color) {
  texture_t tex = std::make_shared<Texture>();
  GLubyte data[] = {color.r, color.g, color.b, 255};
  tex->data = data;
  tex->width = 1;
  tex->height = 1;
  tex->channel = 4;
  tex->SetupTexture();
  return tex;
}

texture_t Texture::NewTexture(const std::string& path, TextureType type) {
  auto it = TextureCollections.find(path);
  if (it != TextureCollections.end()) return it->second;
  texture_t tex = std::make_shared<Texture>();
  stbi_set_flip_vertically_on_load(1);
  tex->data =
      stbi_load(path.c_str(), &tex->width, &tex->height, &tex->channel, 0);
  if (tex->data == NULL) return nullptr;
  tex->type = type;
  tex->SetupTexture();
  fprintf(stdout, "Texutre: Generate texture id %d for %s\n", tex->texture_id,
          path.c_str());
  TextureCollections[path] = tex;
  return tex;
}

std::shared_ptr<Texture> Texture::NewTextureWithTextureId(int tex_id) {
  texture_t tex = std::make_shared<Texture>();
  tex->texture_id = tex_id;
  fprintf(stdout, "Texutre: Generate texture with id %d\n", tex->texture_id);
  return tex;
}

std::shared_ptr<Texture> Texture::NewColorTexture(const Vec3& color) {
  return GenerateColorTexture(color);
}

Material::Material(const std::string& shader_path) {
  this->shader = Shader::NewShader(shader_path);
  assert(this->shader != nullptr);
}

static void UpdateShaderProperty(shared_ptr<Shader> shader,
                                 const std::string name, const std::any& value,
                                 int* texture_unit_index) {
  int loc;
  if (value.type() == typeid(float)) {
    auto raw_value = std::any_cast<float>(value);
    loc = shader->GetUniformLocation(name.c_str());
    if (loc >= 0) glUniform1f(loc, raw_value);
    CHECK_GL_ERROR;
  } else if (value.type() == typeid(glm::vec3)) {
    auto raw_value = std::any_cast<glm::vec3>(value);
    loc = shader->GetUniformLocation(name.c_str());
    if (loc >= 0) glUniform3fv(loc, 1, glm::value_ptr(raw_value));
    CHECK_GL_ERROR;
  } else if (value.type() == typeid(glm::vec4)) {
    auto raw_value = std::any_cast<glm::vec4>(value);
    loc = shader->GetUniformLocation(name.c_str());
    if (loc >= 0) glUniform3fv(loc, 1, glm::value_ptr(raw_value));
    CHECK_GL_ERROR;
  } else if (value.type() == typeid(glm::mat4)) {
    auto raw_value = std::any_cast<glm::mat4>(value);
    loc = shader->GetUniformLocation(name.c_str());
    if (loc >= 0)
      glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(raw_value));
    CHECK_GL_ERROR;
  } else if (value.type() == typeid(glm::mat3)) {
    auto raw_value = std::any_cast<glm::mat3>(value);
    loc = shader->GetUniformLocation(name.c_str());
    if (loc >= 0)
      glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(raw_value));
    CHECK_GL_ERROR;
  } else if (value.type() == typeid(std::shared_ptr<Texture>)) {
    auto raw_value = std::any_cast<std::shared_ptr<Texture>>(value);
    if (shader->SetUniformValues(name.c_str(), *texture_unit_index) >= 0) {
      CHECK_GL_ERROR;
      glActiveTexture(GL_TEXTURE0 + *texture_unit_index);
      CHECK_GL_ERROR;
      if (raw_value->is_cube_map) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, raw_value->texture_id);
        CHECK_GL_ERROR;
      } else {
        glBindTexture(GL_TEXTURE_2D, raw_value->texture_id);
        CHECK_GL_ERROR;
      }
      (*texture_unit_index)++;
    } else {
      printf("active texutre sampler unit %d failed.\n", *texture_unit_index);
      // assert(0);
    }
  } else {
    assert(0);
  }
}

void Material::UpdateShaderUniforms() {
  // TODO...
  SetProperty("phong_exponent", 5.0f);

  int texture_unit_index = 0;
  auto it = properties_.begin();
  while (it != properties_.end()) {
    UpdateShaderProperty(shader, it->first, it->second, &texture_unit_index);
    it++;
  }
}

void Material::DefineValue(const std::string& name) {
  shader->DefineValue(name);
}

void Material::DefineValue(const std::string& name, int val) {
  shader->DefineValue(name, val);
}

void Material::UseShader() {
  shader->Use();
  UpdateShaderUniforms();
}

void Material::SetDiffuseTexture(texture_t tex) {
  DefineValue(DIFFUSE_MACRO);
  SetProperty(DIFFUSE_TEXTURE, tex);
}

void Material::SetNormalTexture(texture_t tex) {
  DefineValue(NORMAL_MACRO);
  SetProperty(NORMAL_TEXTURE, tex);
}
void Material::SetSpecularTexture(texture_t tex) {
  DefineValue(SPECULAR_MACRO);
  SetProperty(SPECULAR_TEXTURE, tex);
}

void Material::SetParams(const MaterialParams& params) {
  SetProperty("uShininess", params.Shininess);
  SetProperty("uAlbedo", params.Albedo);
  SetProperty("uSpecular", params.Specular);

  if (params.DiffuseTexture != "") {
    auto tex = Texture::NewTexture(params.DiffuseTexture, kMainTex);
    SetDiffuseTexture(tex);
  }
  if (params.SpecularTexture != "") {
    auto tex = Texture::NewTexture(params.SpecularTexture, kMainTex);
    SetSpecularTexture(tex);
  }
  if (params.DiffuseTexturePtr != nullptr) {
    SetDiffuseTexture(params.DiffuseTexturePtr);
  }
  if (params.SpecularTexturePtr != nullptr) {
    SetDiffuseTexture(params.SpecularTexturePtr);
  }
}

void MaterialStart() {}

void MaterialFinish() {
  TextureCollections.clear();
  MaterialCollections.clear();
}
