#ifndef __LEARNOPENGL_COMMON_TRANSFORM_H__
#define __LEARNOPENGL_COMMON_TRANSFORM_H__

#include <vector>

#include <glm/gtx/matrix_decompose.hpp>
#include "lo_common.h"

class Transform {
 public:
  Transform() {
    set_position(Vec3(0, 0, 0));
    set_scale(Vec3(1.0, 1.0, 1.0));
    set_roatation(Vec3(0, 0, 0));
  }
  void set_position(const Vec3& v) { position_ = v; }
  void set_scale(const Vec3& v) { scale_ = v; }
  void set_roatation(const Vec3& v) { rotation_ = v; }

  void add_position(const Vec3& v) { position_ = position_ + v; }
  void add_scale(const Vec3& v) { scale_ = scale_ + v; }
  void add_rotation(const Vec3& v) { rotation_ = rotation_ + v; }

  Vec3 position() { return position_; }
  Vec3 scale() { return scale_; }
  Vec3 rotation() { return rotation_; }

  Mat4 model() { return model_; }
  void set_model(const Mat4& v) {
    model_ = v;
    has_model_ = true;

    glm::mat4 transformation;  // your transformation matrix.
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(v, scale, rotation, translation, skew,
                   perspective);

    position_ = translation;
    scale_ = scale;
  }

  bool has_model() { return has_model_; }

 protected:
  Vec3 position_;
  Vec3 scale_;
  Vec3 rotation_;
  Mat4 model_;
  bool has_model_ = false;
};

#endif  // __LEARNOPENGL_COMMON_TRANSFORM_H__