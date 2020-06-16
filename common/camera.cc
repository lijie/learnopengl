
#include "camera.h"

Camera::Camera(const Vec3& pos, const Vec3& up, double fov) {
  position_ = pos;
  world_up_ = up;
  fov_ = fov;

  UpdateVectors();
}

Mat4 Camera::GetViewMatrix() {
  return glm::lookAt(position_, position_ + front_, up_);
}

void Camera::UpdateVectors() {
  Vec3 direction;

  // 按照 euler angler 旋转后, 更新 front
  direction.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
  direction.y = sin(glm::radians(pitch_));
  direction.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));

  front_ = glm::normalize(direction);

  // 右手准则
  right_ = glm::normalize(glm::cross(world_up_, front_));
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

    yaw_ += offset_x;
    pitch_ += offset_y;

    if (pitch_ > 89.0) {
        pitch_ = 89.0;
    }
    if (pitch_ < -89.0) {
        pitch_ = -89.0;
    }

    UpdateVectors();
}
