
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef glm::vec3 Vec3;
typedef glm::mat4 Mat4;

class Camera {
 public:
  Camera(const Vec3& pos, const Vec3& up, double fov);
  Mat4 GetViewMatrix();
  double fov() { return fov_; };

  private:
    Vec3 position_;
    Vec3 up_;
    Vec3 world_up_;
    Vec3 right_;
    Vec3 front_;
    double fov_;

    void UpdateVectors();
};

Camera::Camera(const Vec3& pos, const Vec3& up, double fov) {
    position_ = pos;
    world_up_ = up;
    fov_ = fov;
}

Mat4 Camera::GetViewMatrix() {

}

void Camera::UpdateVectors() {
    
}