#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "shader.h"

#define ERROR(fmt, ...) fprintf(stderr, "ERROR:SHADER[%s:%d]:" fmt, __func__, __LINE__, ##__VA_ARGS__)

namespace lavender {

static const char * read_file(const char *path) {
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


class Shader {
 public:
  static Shader* Create();
  static Shader* CreateWithSource();
  void Release();

  uint32_t program() { return program_; }

 private:
  uint32_t vertex_shader_ = 0;
  uint32_t fragment_shader_ = 0;
  uint32_t program_ = 0;
  const char *vertex_source_ = NULL;
  const char *fragment_source_ = NULL;
#define SHADER_FLAGS_EXTERNAL_SOURCE (1 << 0)
  uint32_t flags_ = 0;

  int ReadShaderFile(struct shader *shader, const char *vertex_path, const char *fragment_path);
  int CheckShaderCompileSuccess(uint32_t shader);
  int CheckShaderLinkSuccess(uint32_t program);

  Shader() {}
  ~Shader() {}
};

int Shader::ReadShaderFile(struct shader *shader, const char *vertex_path, const char *fragment_path) {
  shader->vertex_source = read_file(vertex_path);
  shader->fragment_source = read_file(fragment_path);
  shader->flags &= ~(SHADER_FLAGS_EXTERNAL_SOURCE);
  return 0;
}

Shader::~Shader() {
  if (!(flags_ & SHADER_FLAGS_EXTERNAL_SOURCE)) {
    free((void *)vertex_source_);
    free((void *)fragment_source_);
  }
}

}

int Shader::CheckShaderCompileSuccess(uint32_t shader) {
  int success = 0;
  char info_log[512];

  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, info_log);
    fprintf(stderr, "ERROR: SHADER: %s\n", info_log);
  }

  return success;
}

int Shader::CheckShaderLinkSuccess(uint32_t program)
{
  int success = 0;
  char info_log[512];

  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, NULL, info_log);
    fprintf(stderr, "ERROR: PROGRAM: %s\n", info_log);
  }

  return success;
}

int Shader::CompileAndLink() {
  uint32_t vertex_shader;
  uint32_t fragment_shader;
  uint32_t shader_program;
  int res = 0;
	
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_source_, NULL);
  glCompileShader(vertex_shader);

  if (!CheckShaderCompileSuccess(vertex_shader)) {
    ERROR("vertex shader compile error.\n");
    res = -1;
    goto out;
  }

  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_source_, NULL);
  glCompileShader(fragment_shader);

  if (!CheckShaderCompileSuccess(fragment_shader)) {
    ERROR("fragment shader compile error.\n");
    res = -1;
    goto out;
  }

  shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);

  if (!CheckShaderLinkSuccess(shader_program)) {
    ERROR("shader link error.\n");
    res = -1;
    goto out;
  }

  fragment_shader_ = fragment_shader;
  vertex_shader_ = vertex_shader;
  program_ = shader_program;

out:
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  return res;
}

Shader * Shader::CreateWithSource(const char *vertex_source, const char *fragment_source) {
  Shader *shader = new Shader;
  shader->vertex_source_ = vertex_source;
  shader->fragment_source_ = fragment_source;

  if (shader->CompileAndLink(shader) != 0) {
    ERROR("shader compile error");
    delete shader;
    return NULL;
  }

  return shader;
}

Shader * Shader::Create(const char *vertex_path, const char *fragment_path) {
  Shader *shader = new Shader;

  if (shader->ReadShaderFile(vertex_path, fragment_path) != 0) {
    ERROR("read file %s, %s failed.\n", vertex_path, fragment_path);
    goto err_out;
  }

  if (shader->CompileAndLink() != 0) {
    ERROR("shader compile error.\n");
    goto err_out;
  }

  return shader;
err_out:
  delete shader;
  return NULL;
}

void Shader::Use() {
  glUseProgram(program_);
}

int shader::GetUniformLocation(const char *name) {
  return glGetUniformLocation(program_, name);
}

void Shader::Release() {
  delete this;
}

