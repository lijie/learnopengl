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

Texture::~Texture() { glDeleteTextures(1, (const GLuint*)&texture_id); }

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
  if (format == GL_RGBA)
    wrap = GL_CLAMP_TO_EDGE;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  texture_id = id;
}

static texture_t GenerateDefaultWhiteTexture() {
  texture_t tex = std::make_shared<Texture>();
  GLubyte data[] = {255, 255, 255, 255};
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
  return tex;
}

Material::Material(const std::string& shader_path) {
  this->shader = Shader::NewShader(shader_path);
  assert(this->shader != nullptr);
}

static void UpdateShaderProperty(shared_ptr<Shader> shader,
                                 const std::string name,
                                 const std::any& value) {
  int loc;
  int texteure_unit_index = 0;
  if (value.type() == typeid(float)) {
    auto raw_value = std::any_cast<float>(value);
    loc = shader->GetUniformLocation(name.c_str());
    if (loc >= 0) glUniform1f(loc, raw_value);
  } else if (value.type() == typeid(glm::vec3)) {
    auto raw_value = std::any_cast<glm::vec3>(value);
    loc = shader->GetUniformLocation(name.c_str());
    if (loc >= 0) glUniform3fv(loc, 1, glm::value_ptr(raw_value));
  } else if (value.type() == typeid(glm::mat4)) {
    auto raw_value = std::any_cast<glm::mat4>(value);
    loc = shader->GetUniformLocation(name.c_str());
    if (loc >= 0)
      glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(raw_value));
  } else if (value.type() == typeid(glm::mat3)) {
    auto raw_value = std::any_cast<glm::mat3>(value);
    loc = shader->GetUniformLocation(name.c_str());
    if (loc >= 0)
      glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(raw_value));
  } else if (value.type() == typeid(std::shared_ptr<Texture>)) {
    auto raw_value = std::any_cast<std::shared_ptr<Texture>>(value);
    if (shader->SetUniformValues(name.c_str(), texteure_unit_index) >= 0) {
      glActiveTexture(GL_TEXTURE0 + texteure_unit_index);
      glBindTexture(GL_TEXTURE_2D, raw_value->texture_id);
      texteure_unit_index++;
    }
  } else {
    assert(0);
  }
}

void Material::UpdateShaderUniforms(Transform* t) {
  // TODO...
  SetProperty("phong_exponent", 5.0f);

  auto it = properties_.begin();
  while (it != properties_.end()) {
    UpdateShaderProperty(shader, it->first, it->second);
    it++;
  }
}

void Material::UseShader(Transform* t) {
  shader->Use();
  UpdateShaderUniforms(t);
}
