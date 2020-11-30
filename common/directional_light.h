#ifndef __LEARNOPENGL_COMMON_DIRECTIONAL_LIGHT_H__
#define __LEARNOPENGL_COMMON_DIRECTIONAL_LIGHT_H__

#include <memory>

#include "lo_common.h"
#include "transform.h"

enum LightType {
  kDirectionalLight = 1,
  kPointLight = 2,
};

class Material;

class Light {
 public:
  Light(LightType type) : light_type_(type) {}
  virtual void SetUniforms(std::shared_ptr<Material> mat){};

  Transform *GetTransform() { return &transform_; }

 protected:
  Transform transform_;
  LightType light_type_;
};

class Material;
class DirectionalLight : public Light {
 public:
  DirectionalLight(): Light(kDirectionalLight) {}
  DirectionalLight(const Vec3& dir, const Vec3& c)
      : Light(kDirectionalLight), direction_(dir), color_(c) {}

  void set_direction(const Vec3& dir) { direction_ = dir; }
  void set_color(const Vec3& c) { color_ = c; }
  void set_rotation(float angle, Vec3 axis);

  const Vec3& direction() { return direction_; }
  const Vec3& color() { return color_; }

  void SetUniforms(std::shared_ptr<Material> mat);

 private:
  Vec3 direction_;
  Vec3 color_;
};

template<typename T, typename ...Ts>
std::shared_ptr<T> NewLight(Ts ...args) {
  return std::make_shared<T>(args...);
}

class LightManager {
 public:
  void AddLight(std::shared_ptr<Light> light);
};

#endif  // __LEARNOPENGL_COMMON_DIRECTIONAL_LIGHT_H__
