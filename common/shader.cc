#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "shader.h"

#define ERROR(fmt, ...) fprintf(stderr, "ERROR:SHADER[%s:%d]:" fmt, __func__, __LINE__, ##__VA_ARGS__)

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
  vertex_source_ = read_file(v_path);
  fragment_source_ = read_file(f_path);
  flags_ &= ~(SHADER_FLAGS_EXTERNAL_SOURCE);
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
  res = true;

out:
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  return res;
}

void Shader::Use() {
  glUseProgram(program_);
}

int Shader::GetUniformLocation(const char *name) {
  return glGetUniformLocation(program_, name);
}
