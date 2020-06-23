
#include "camera.h"

Camera::Camera(const Vec3& pos, const Vec3& up, double fov, double aspect_ratio) {
  position_ = pos;
  world_up_ = up;
  fov_ = fov;
  aspect_ratio_ = aspect_ratio;

  UpdateVectors();
}

Mat4 Camera::GetViewMatrix() {
  return glm::lookAt(position_, position_ + front_, up_);
}

Mat4 Camera::GetProjectionMatrix() {
    return glm::perspective(glm::radians(fov()), aspect_ratio_, 0.1, 100.0);
}

void Camera::UpdateVectors() {
  Vec3 direction;

  // 按照 euler angler 旋转后, 更新 front
  direction.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
  direction.y = sin(glm::radians(pitch_));
  direction.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));

  front_ = glm::normalize(direction);

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
