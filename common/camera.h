#ifndef __LEARNOPENGL_COMMON__
#define __LEARNOPENGL_COMMON__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef glm::vec3 Vec3;
typedef glm::mat4 Mat4;

enum CameraKey {
    kCameraKeyForward = 1,
    kCameraKeyBackward,
    kCameraKeyLeft,
    kCameraKeyRight,
};

class Camera {
 public:
  Camera(const Vec3& pos, const Vec3& up, double fov);
  Mat4 GetViewMatrix();
  double fov() { return fov_; };

  void ProcessKey(CameraKey key, double delta_time);
  void ProcessMouse(double delta_x, double delta_y);

  private:
    Vec3 position_;
    Vec3 up_;
    Vec3 world_up_;
    Vec3 right_;
    Vec3 front_;

    // 默认情况下, 旋转Y轴 -90, 看向 -Z 方向
    double yaw_ = -90;
    double pitch_ = 0;
    double roll_ = 0;

    double fov_;

    double move_speed_ = 2.5;
    double mosue_sensitivity_ = 0.1;

    void UpdateVectors();
};

#endif  // __LEARNOPENGL_COMMON__