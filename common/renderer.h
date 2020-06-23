#ifndef __LEARNOPENGL_RENDERER_H__
#define __LEARNOPENGL_RENDERER_H__

class GlContext;

class Renderer {
 public:
  virtual void Render(GlContext *) = 0;
};

#endif  // __LEARNOPENGL_RENDERER_H__
