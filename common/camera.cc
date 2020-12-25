
#include "camera.h"

#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/string_cast.hpp"

Camera::Camera(const Vec3& pos, const Vec3& up, const Vec3& target, double fov,
               double aspect_ratio) {
  position_ = pos;
  world_up_ = up;
  fov_ = fov;
  aspect_ratio_ = aspect_ratio;

  look_target_ = target;

  UpdateVectors();
}

Mat4 Camera::GetViewMatrix() {
  // return glm::lookAt(position_, position_ + front_, up_);
  // printf("GetViewMatrix %s\n%s\n", glm::to_string(position_).c_str(), glm::to_string(position_ + front_).c_str());
  return glm::lookAt(position_, position_ + front_, world_up_);
}

Mat4 Camera::GetProjectionMatrix() {
  return glm::perspective(glm::radians(fov()), aspect_ratio_, 0.1, 1000.0);
}

void Camera::set_position(const Vec3& pos) {
  position_ = pos;
  UpdateVectors();
}

void Camera::set_target(const Vec3& target) {
  look_target_ = target;
  UpdateVectors();
}

void Camera::UpdateVectors() {
  Vec3 direction = look_target_ - position_;

#if 0
  // 按照 euler angler 旋转后, 更新 front
  direction.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
  direction.y = sin(glm::radians(pitch_));
  direction.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
#endif

  front_ = glm::normalize(direction);
  // printf("camera front: %s\n", glm::to_string(front_).c_str());
  // printf("position: %s\n", glm::to_string(position_).c_str());

  // 右手准则
  right_ = glm::normalize(glm::cross(front_, world_up_));
  up_ = glm::normalize(glm::cross(right_, front_));
}

void Camera::ProcessKey(CameraKey key, double delta_time) {
  double velocity = move_speed_ * delta_time;

  switch (key) {
    case kCameraKeyForward:
      position_ += front_ * (float)velocity;
      break;
    case kCameraKeyBackward:
      position_ -= front_ * (float)velocity;
      break;
    case kCameraKeyLeft:
      position_ -= right_ * (float)velocity;
      break;
    case kCameraKeyRight:
      position_ += right_ * (float)velocity;
      break;
  }
}

void Camera::ProcessMouse(double delta_x, double delta_y) {
  double offset_x = delta_x * mosue_sensitivity_;
  double offset_y = delta_y * mosue_sensitivity_;

  yaw_ = -offset_x;
  pitch_ = -offset_y;

  if (pitch_ > 89.0) {
    pitch_ = 89.0;
  }
  if (pitch_ < -89.0) {
    pitch_ = -89.0;
  }

  if (yaw_ > 89.0) {
    yaw_ = 89.0;
  }
  if (yaw_ < -89.0) {
    yaw_ = -89.0;
  }

  if (abs(offset_x) > abs(offset_y)) {
    Vec3 direction = position_ - look_target_;
    direction =
        glm::rotate(direction, float(glm::radians(yaw_)), Vec3(0, 1, 0));
    position_ = direction;
  } else  {
    Vec3 direction = position_ - look_target_;
    direction =
        glm::rotate(direction, float(glm::radians(pitch_)), Vec3(1, 0, 0));
    position_ = direction;
  }

  UpdateVectors();
}
