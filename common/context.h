#ifndef __TEST_OPENGL_CONTEXT_H__
#define __TEST_OPENGL_CONTEXT_H__

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

class Shader;

class GlContext {
 public:
  uint32_t vao = 0;
  uint32_t vbo = 0;
  uint32_t ebo = 0;
  Shader *shader = nullptr;
#define TEXTURE_ID_NR 16
  int texture_ids[TEXTURE_ID_NR];
};

class Camera;
class Scene {
  public:
    void AddCamera(Camera *c) { camera_ = c; }
    Camera *GetCamera() { return camera_; }
  private:
    Camera *camera_;
};

#endif

