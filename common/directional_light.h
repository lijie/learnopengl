#ifndef __LEARNOPENGL_COMMON_DIRECTIONAL_LIGHT_H__
#define __LEARNOPENGL_COMMON_DIRECTIONAL_LIGHT_H__

#include <memory>
#include "lo_common.h"

class Light {
 public:
    
};

class Material;
class DirectionalLight : public Light {
 public:
    DirectionalLight() {}
    DirectionalLight(const Vec3& dir, const Vec3& c): direction_(dir), color_(c) {}

    void set_direction(const Vec3& dir) { direction_ = dir; }
    void set_color(const Vec3& c) { color_ = c; }

    const Vec3& direction() { return direction_; }
    const Vec3& color() { return color_; }

    void SetUniforms(std::shared_ptr<Material> mat);
 private:
    Vec3 direction_;
    Vec3 color_;
};

#endif // __LEARNOPENGL_COMMON_DIRECTIONAL_LIGHT_H__
