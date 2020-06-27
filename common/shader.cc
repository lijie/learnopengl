#include "shader.h"

#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <map>
#include <string>

#include "camera.h"
#include "glad/glad.h"
#include "lo_common.h"
#include "scene.h"

#define ERROR(fmt, ...) \
  fprintf(stderr, "ERROR:SHADER[%s:%d]:" fmt, __func__, __LINE__, ##__VA_ARGS__)

static std::map<std::string, shader_t> ShaderCollections;

#if 0
struct shader {
  uint32_t vertex_shader;
  uint32_t fragment_shader;
  uint32_t program;
  const char *vertex_source;
  const char *fragment_source;
#define SHADER_FLAGS_EXTERNAL_SOURCE (1 << 0)
  uint32_t flags;
};
#endif

std::shared_ptr<Shader> Shader::NewShader(const std::string &shader_name) {
  auto it = ShaderCollections.find(shader_name);
  if (it != ShaderCollections.end()) return it->second;

  std::string v_path = shader_name + "_vertex.glsl";
  std::string f_path = shader_name + "_fragment.glsl";

  auto shader = std::make_shared<Shader>();
  shader->Open(v_path.c_str(), f_path.c_str());
  bool res = shader->CompileAndLink();
  if (!res) {
    return nullptr;
  }
  ShaderCollections[shader_name] = shader;
  return shader;
}

static const char *read_file(const char *path) {
  FILE *f = fopen(path, "r");
  assert(f != NULL);

  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  rewind(f);

  char *ptr = (char *)malloc(size + 1);
  assert(ptr != NULL);

  fread(ptr, size, 1, f);
  ptr[size] = 0;
  return ptr;
}

Shader::~Shader() {
  if (flags_ & SHADER_FLAGS_EXTERNAL_SOURCE) {
    free((void *)vertex_source_);
    free((void *)fragment_source_);
  }
}

Shader::Shader(const char *v_src, const char *f_src) {
  vertex_source_ = v_src;
  fragment_source_ = f_src;
  flags_ |= SHADER_FLAGS_EXTERNAL_SOURCE;
}

bool Shader::Open(const char *v_path, const char *f_path) {
  if (flags_ & SHADER_FLAGS_ALREADY_READ) {
    return true;
  }
  vertex_source_ = read_file(v_path);
  fragment_source_ = read_file(f_path);
  flags_ &= ~(SHADER_FLAGS_EXTERNAL_SOURCE);
  flags_ |= SHADER_FLAGS_ALREADY_READ;
  return true;
}

int Shader::CheckCompileSuccess(uint32_t shader) {
  int success = 0;
  char info_log[512];

  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, info_log);
    fprintf(stderr, "ERROR: SHADER: %s\n", info_log);
  }

  return success;
}

int Shader::CheckLinkSuccess(uint32_t program) {
  int success = 0;
  char info_log[512];

  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, NULL, info_log);
    fprintf(stderr, "ERROR: PROGRAM: %s\n", info_log);
  }

  return success;
}

bool Shader::CompileAndLink() {
  uint32_t vertex_shader;
  uint32_t fragment_shader;
  uint32_t shader_program;
  bool res = false;

  if (flags_ & SHADER_FLAGS_ALREADY_COMPILE) {
    return true;
  }

  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_source_, NULL);
  glCompileShader(vertex_shader);

  if (!CheckCompileSuccess(vertex_shader)) {
    ERROR("vertex shader compile error.\n");
    goto out;
  }

  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_source_, NULL);
  glCompileShader(fragment_shader);

  if (!CheckCompileSuccess(fragment_shader)) {
    ERROR("fragment shader compile error.\n");
    goto out;
  }

  shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);

  if (!CheckLinkSuccess(shader_program)) {
    ERROR("shader link error.\n");
    goto out;
  }

  fragment_shader_ = fragment_shader;
  vertex_shader_ = vertex_shader;
  program_ = shader_program;
  flags_ |= SHADER_FLAGS_ALREADY_COMPILE;
  res = true;

out:
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  return res;
}

void Shader::Use() { glUseProgram(program_); }

int Shader::GetUniformLocation(const char *name) {
  return glGetUniformLocation(program_, name);
}

void Shader::InitMatrixUniforms(Transform *transform) {
  glm::mat4 model = glm::mat4(1.0);

  if (transform->has_model()) {
    model = transform->model();
  } else {
    model = glm::rotate(model, glm::radians(transform->rotation().x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(transform->rotation().y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(transform->rotation().z), glm::vec3(0, 0, 1));

    model = glm::translate(model, transform->position());
    model = glm::scale(model, transform->scale());
    // model = glm::rotate(model, glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f,
    // 0.5f)); model = glm::translate(model, cube_positions[i]);
  }

  auto view = GetWorld()->GetCamera()->GetViewMatrix();
  auto projection = GetWorld()->GetCamera()->GetProjectionMatrix();
  auto mvp = projection * view * model;
  auto mv3x3 = glm::mat3(view * model);
  auto normal_model = glm::mat3(glm::transpose(glm::inverse(model)));

  int loc = GetUniformLocation("model");
  if (loc >= 0) glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model));

  loc = GetUniformLocation("view");
  if (loc >= 0) glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view));

  loc = GetUniformLocation("projection");
  if (loc >= 0) glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection));

  loc = GetUniformLocation("mvp");
  if (loc >= 0) glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mvp));

  loc = GetUniformLocation("mv3x3");
  if (loc >= 0) glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(mv3x3));

  loc = GetUniformLocation("normal_model");
  if (loc >= 0) glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(normal_model));

  auto light_source = GetWorld()->GetLightSource();
  auto light_position = light_source->position();
  loc = GetUniformLocation("light_position");
  if (loc > 0) glUniform3fv(loc, 1, glm::value_ptr(light_position));

  auto light_power = light_source->power();
  loc = GetUniformLocation("light_power");
  if (loc >= 0) glUniform1f(loc, light_power);

  auto light_color = light_source->color();
  loc = GetUniformLocation("light_color");
  if (loc >= 0) glUniform3fv(loc, 1, glm::value_ptr(light_color));

  auto camera_position = GetWorld()->GetCamera()->position();
  loc = GetUniformLocation("camera_position");
  if (loc >= 0) glUniform3fv(loc, 1, glm::value_ptr(camera_position));
}
