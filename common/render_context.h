#ifndef __LEARNOPENGL_RENDER__CONTEXT_H__
#define __LEARNOPENGL_RENDER__CONTEXT_H__

#include "lo_common.h"

class Framebuffer;

struct SceneCommonUniforms {
  Mat4 ViewMatrix;
  Mat4 ProjectionMatrix;
};

struct RenderContext {
    SceneCommonUniforms CommonUniforms;
    MaterialPtr ContextMaterial;
    std::shared_ptr<Framebuffer> ShadowPassOutputFramebuffer;
    bool EnableLight;
    bool EnableShadow;
};

#endif  // __LEARNOPENGL_RENDER__CONTEXT_H__