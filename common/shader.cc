#include "shader.h"
#include "shader_include.h"

#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <map>
#include <sstream>
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
  shader->shader_name_ = shader_name;
#if 0
  bool res = shader->CompileAndLink();
  if (!res) {
    return nullptr;
  }
  ShaderCollections[shader_name] = shader;
#endif
  return shader;
}

static const char *read_file(const char *path) {
  FILE *f = fopen(path, "r");
  if (f == NULL) {
    fprintf(stderr, "cannot open file %s\n", path);
    assert(f != NULL);
  }

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
  // if (flags_ & SHADER_FLAGS_EXTERNAL_SOURCE) {
  //   free((void *)vertex_source_);
  //   free((void *)fragment_source_);
  // }
}

// Shader::Shader(const char *v_src, const char *f_src) {
//   vertex_source_ = v_src;
//   fragment_source_ = f_src;
//   flags_ |= SHADER_FLAGS_EXTERNAL_SOURCE;
// }

bool Shader::Open(const char *v_path, const char *f_path) {
  if (flags_ & SHADER_FLAGS_ALREADY_READ) {
    return true;
  }
  vertex_source_ = Shadinclude::load(std::string(v_path));
  fragment_source_ = Shadinclude::load(std::string(f_path));

  // vertex_source_ = read_file(v_path);
  // fragment_source_ = read_file(f_path);
  // flags_ &= ~(SHADER_FLAGS_EXTERNAL_SOURCE);
  // flags_ |= SHADER_FLAGS_ALREADY_READ;
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

  // printf("use shader %s\n", shader_name_.c_str());
  if (flags_ & SHADER_FLAGS_ALREADY_COMPILE) {
    return true;
  }

  std::string v_src;
  std::string f_src;

  v_src = defined_values_ + vertex_source_;
  f_src = defined_values_ + fragment_source_;

  const char *v_src_ptr = v_src.c_str();
  const char *f_src_ptr = f_src.c_str();

  // printf("%s\n", v_src_ptr);
  // printf("%s\n", f_src_ptr);

  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  CHECK_GL_ERROR;
  glShaderSource(vertex_shader, 1, &v_src_ptr, NULL);
  CHECK_GL_ERROR;
  glCompileShader(vertex_shader);
  CHECK_GL_ERROR;

  if (!CheckCompileSuccess(vertex_shader)) {
    ERROR("vertex shader compile error.\n");
    goto out;
  }

  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  CHECK_GL_ERROR;
  glShaderSource(fragment_shader, 1, &f_src_ptr, NULL);
  CHECK_GL_ERROR;
  glCompileShader(fragment_shader);
  CHECK_GL_ERROR;

  if (!CheckCompileSuccess(fragment_shader)) {
    ERROR("fragment shader compile error.\n");
    goto out;
  }

  shader_program = glCreateProgram();
  CHECK_GL_ERROR;
  glAttachShader(shader_program, vertex_shader);
  CHECK_GL_ERROR;
  glAttachShader(shader_program, fragment_shader);
  CHECK_GL_ERROR;
  glLinkProgram(shader_program);
  CHECK_GL_ERROR;

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

void Shader::Use() {
  bool res = CompileAndLink();
  assert(res != false);
  glUseProgram(program_);
  CHECK_GL_ERROR;
}

int Shader::GetUniformLocation(const char *name) {
  return glGetUniformLocation(program_, name);
}

int Shader::GetAttribLocation(const char *name) {
  return glGetAttribLocation(program_, name);
}

void Shader::DefineValue(const std::string &name) {
  defined_values_ = defined_values_ + "#define " + name + "\n";
}

void Shader::DefineValue(const std::string &name, int val) {
  std::stringstream ss;
  ss << val;
  std::string s = "#define " + name + " " + ss.str() + "\n";
  defined_values_ = defined_values_ + s;
}
