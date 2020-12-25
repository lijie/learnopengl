#ifndef __LEARNOPENGL_COMMON_DIRECTIONAL_LIGHT_H__
#define __LEARNOPENGL_COMMON_DIRECTIONAL_LIGHT_H__

#include <memory>
#include <vector>
#include <functional>

#include "lo_common.h"
#include "transform.h"
#include "renderer.h"

enum LightType {
  kDirectionalLight = 1,
  kPointLight = 2,
  kAmbientLight = 3,
  kSpotLight = 4,
};

struct SceneCommonUniforms;
struct RenderContext;

struct ShadowParams {
  Vec2 Resolution;
};

struct ShadowMapData {
  FramebufferPtr DepthMapBuffer;
};

class Light {
 public:
  Light(LightType type) : light_type_(type) {}
  virtual void SetUniforms(const RenderContext& ctx, std::shared_ptr<Material> mat,
                           const SceneCommonUniforms& common_uniforms){};

  Transform* GetTransform() { return &transform_; }
  LightType light_type() { return light_type_; }
  int index() { return index_; }
  void set_index(int v) { index_ = v; }
  int shadow_index() { return shadow_index_; }
  void set_shadow_index(int v) { shadow_index_ = v; }
  const Vec3& color() { return color_; }
  void set_color(const Vec3& color) { color_ = color; }

  // virtual void PrepareShadowMapData(const ShadowParams& params, ShadowMapData *data) {};

  void EanbleShadow(const ShadowParams& params) {
    enable_shadow_ = true;
    shadow_params_ = params;
    // PrepareShadowMapData(shadow_params_, &shadow_map_data_);
  }
  void DisableShadow() {
    enable_shadow_ = false;
  }
  bool IsShadowEnabled() { return enable_shadow_; }
  virtual RenderContext *GetDepthMapContext(RenderPass pass) { return nullptr; }
  
 protected:
  Transform transform_;
  LightType light_type_;
  ShadowParams shadow_params_;
  ShadowMapData shadow_map_data_;
  float intensity_ = 1.0f;
  Vec3 color_;
  int index_ = 0;
  int shadow_index_ = 0;
  bool enable_shadow_ = false;

  RenderContext *AllocAndInitDepthMapContext();
};

class DirectionalLight : public Light {
 public:
  DirectionalLight() : Light(kDirectionalLight) {}
  DirectionalLight(const Vec3& target, const Vec3& c)
      : Light(kDirectionalLight), target_(target) {
    set_color(c);
  }

  void set_target(const Vec3& target) { target_ = target; }

  const Vec3& target() { return target_; }
  Vec3 direction() { return target_ - transform_.position(); }
  const Vec3& color() { return color_; }

  void SetUniforms(const RenderContext& ctx, MaterialPtr mat,
                   const SceneCommonUniforms& common_uniforms) override;

 private:
  // Vec3 direction_;
  Vec3 target_;
};

class PointLight : public Light {
 public:
  PointLight(float distance, float decay, const Vec3& color)
      : Light(kPointLight), distance_(distance), decay_(decay) {
    set_color(color);
  }

  void SetUniforms(const RenderContext& ctx, MaterialPtr mat,
                   const SceneCommonUniforms& common_uniforms) override;

 private:
  float distance_;
  float decay_;
};

typedef std::shared_ptr<PointLight> PointLightPtr;

class AmbientLight : public Light {
 public:
  AmbientLight() : Light(kAmbientLight) {}
  AmbientLight(const Vec3& color) : Light(kAmbientLight), color_(color) {}
  const Vec3& color() { return color_; }

  void SetUniforms(const RenderContext& ctx, MaterialPtr mat,
                   const SceneCommonUniforms& common_uniforms) override;

 private:
  Vec3 color_;
};

class SpotLight : public Light {
 public:
  SpotLight(float distance, float decay, const Vec3& color,
            float cone = glm::pi<float>() / 6.0f, float penumbra = 0.0f)
      : Light(kSpotLight),
        distance_(distance),
        decay_(decay),
        cone_(cone),
        penumbra_(penumbra) {
    set_color(color);
  }

  void set_target(const Vec3& target) { target_ = target; }

  const Vec3& target() { return target_; }
  Vec3 direction() { return target_ - transform_.position(); }
  const Vec3& color() { return color_; }

  void SetUniforms(const RenderContext& ctx, MaterialPtr mat,
                   const SceneCommonUniforms& common_uniforms) override;

  RenderContext *GetDepthMapContext(RenderPass pass) override;

 private:
  Vec3 target_;
  RenderContext *depthmap_ctx_ = nullptr;
  float distance_ = 0.0f;
  float decay_ = 1.0f;
  float cone_ = glm::pi<float>() / 6.0f;  // 本影, 角度(弧度)
  float penumbra_ = 0.0f;  // 半影, [0, 1], 所占本影区域的百分比
  Mat4 light_space_mat_;
};

typedef std::shared_ptr<SpotLight> SpotLightPtr;

class LightHelper : public Renderer {
 public:
  LightHelper(LightPtr light);
  void BeforeUpdate() override;

 private:
  ShapePtr shape_;
  LightPtr light_;
};

class LightManager {
 public:
  void AddLight(LightPtr light);
  void SetUniforms(const RenderContext& ctx, MaterialPtr mat, const SceneCommonUniforms& common_uniforms);

  void ForEachLight(std::function<void(LightPtr)> cb);

 private:
  int directional_light_index_ = 0;
  int directional_shadow_index_ = 0;
  int point_light_index_ = 0;
  int point_shadow_index_ = 0;
  int spot_light_index_ = 0;
  int spot_shadow_index_ = 0;

  std::vector<LightPtr> light_vec_;
};

#endif  // __LEARNOPENGL_COMMON_DIRECTIONAL_LIGHT_H__
