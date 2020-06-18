#ifndef __TEST_OPENGL_SHADER_H__
#define __TEST_OPENGL_SHADER_H__

#include <stdint.h>

class Shader {
 public:
  Shader() {};
  Shader(const char *v_src, const char *f_src);
  ~Shader();
  bool Open(const char *v_path, const char *f_path);
  bool CompileAndLink();
  void Use();
  int GetUniformLocation(const char *name);

  template<typename T = int>
  int SetUniformValues(const char *name, T v1) {
    int location = GetUniformLocation(name);          
    if (location < 0)                                               
      return -1;                                                        
    glUniform1i(location, v1);
    return 0;
  }
  
  template<typename T>
  int SetUniformValues(const char *name, T v1, T v2, T v3, T v4) {
    int location = GetUniformLocation(name);          
    if (location < 0)                                               
      return -1;                                                        
    glUniform4f(location, v1, v2, v3, v4);
    return 0;
  }

  inline uint32_t program() {
    return program_;
  }

 private:
  const char *vertex_source_ = nullptr;
  const char *fragment_source_ = nullptr;
#define SHADER_FLAGS_EXTERNAL_SOURCE (1 << 0)
#define SHADER_FLAGS_ALREADY_READ (1 << 1)
#define SHADER_FLAGS_ALREADY_COMPILE (1 << 2)
  uint32_t flags_ = 0;
  uint32_t vertex_shader_ = 0;
  uint32_t fragment_shader_ = 0;
  uint32_t program_ = 0;

  int CheckCompileSuccess(uint32_t shader);
  int CheckLinkSuccess(uint32_t program);
};

#if 0
struct shader;

struct shader * shader_create_with_source(const char *vertex_source, const char *fragment_source);

struct shader * shader_create(const char *vertex_path, const char *fragment_path);

void shader_use(struct shader *shader);

void shader_close(struct shader *shader);

int shader_get_uniform_location(struct shader *shader, const char *name);

#define __BUILD_SHADER_UNIFORM_FUNC_4(TYPE, SUFFIX)                     \
  static inline int shader_uniform_set_##TYPE##4(struct shader *shader, \
                                                 const char *name,      \
                                                 TYPE v1,               \
                                                 TYPE v2,               \
                                                 TYPE v3,               \
                                                 TYPE v4)               \
  {                                                                     \
    int location = shader_get_uniform_location(shader, name);           \
    if (location < 0)                                                   \
      return -1;                                                        \
    glUniform##SUFFIX(location, v1, v2, v3, v4);                        \
    return 0;                                                           \
  }

__BUILD_SHADER_UNIFORM_FUNC_4(float, 4f)
#endif
#endif
