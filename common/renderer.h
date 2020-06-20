#ifndef __LEARNOPENGL_RENDERER_H__
#define __LEARNOPENGL_RENDERER_H__

class GLContext;

class Renderer {
 public:
  virtual void Draw(GLContext *) = 0;
};

#endif  // __LEARNOPENGL_RENDERER_H__
