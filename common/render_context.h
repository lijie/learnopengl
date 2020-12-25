#ifndef __LEARNOPENGL_RENDER__CONTEXT_H__
#define __LEARNOPENGL_RENDER__CONTEXT_H__

#include <vector>
#include "lo_common.h"

class Framebuffer;
class Camera;

struct SceneCommonUniforms {
  Mat4 ViewMatrix;
  Mat4 ProjectionMatrix;
};

struct RenderContext {
    // Camera *Camera;
    RenderPass Pass;
    MaterialPtr ContextMaterial;
    // std::shared_ptr<Framebuffer> ShadowPassOutputFramebuffer;
    std::shared_ptr<Framebuffer> OutputFramebuffer;
    SceneCommonUniforms CommonUniforms;
    Vec2 Resolution;
    std::vector<int> GLEnableFlags;
    int GLClearFlags;
    bool EnableLight;
    bool EnableShadow;
};

#endif  // __LEARNOPENGL_RENDER__CONTEXT_H__