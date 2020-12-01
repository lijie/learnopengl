#ifndef __LEARNOPENGL_COMMON_DIRECTIONAL_LIGHT_H__
#define __LEARNOPENGL_COMMON_DIRECTIONAL_LIGHT_H__

#include <memory>
#include <vector>

#include "lo_common.h"
#include "transform.h"

enum LightType {
  kDirectionalLight = 1,
  kPointLight = 2,
};

struct SceneCommonUniforms;

class Light {
 public:
  Light(LightType type) : light_type_(type) {}
  virtual void SetUniforms(std::shared_ptr<Material> mat, const SceneCommonUniforms& common_uniforms){};

  Transform *GetTransform() { return &transform_; }
  LightType light_type() { return light_type_; }
  int index() { return index_; }
  void set_index(int v) { index_ = v; }

 protected:
  Transform transform_;
  LightType light_type_;
  int index_ = 0;
};

class DirectionalLight : public Light {
 public:
  DirectionalLight(): Light(kDirectionalLight) {}
  DirectionalLight(const Vec3& target, const Vec3& c)
      : Light(kDirectionalLight), target_(target), color_(c) {}

  void set_target(const Vec3& target) { target_ = target; }
  void set_color(const Vec3& c) { color_ = c; }

  const Vec3& target() { return target_; }
  const Vec3& direction() { return target_ - transform_.position(); }
  const Vec3& color() { return color_; }

  void SetUniforms(MaterialPtr mat, const SceneCommonUniforms& common_uniforms) override;

 private:
  // Vec3 direction_;
  Vec3 target_;
  Vec3 color_;
};

class LightManager {
 public:
  void AddLight(LightPtr light);
  void SetUniforms(MaterialPtr mat, const SceneCommonUniforms& common_uniforms);

  private:
    int directional_light_index_ = 0;
    int point_light_index_ = 0;

    std::vector<LightPtr> light_vec_;
};

#endif  // __LEARNOPENGL_COMMON_DIRECTIONAL_LIGHT_H__
