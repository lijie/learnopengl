#ifndef __TEST_OPENGL_CONTEXT_H__
#define __TEST_OPENGL_CONTEXT_H__

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "material.h"

class Shader;

class GlContext {
 public:
  uint32_t vao = 0;
  uint32_t vbo = 0;
  uint32_t ebo = 0;
  Shader *shader = nullptr;
#define TEXTURE_ID_NR 16
  texture_t texture_ids[TEXTURE_ID_NR];
};

#endif

