#ifndef __LEARNOPENGL_LO_COMMON_H__
#define __LEARNOPENGL_LO_COMMON_H__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef glm::vec3 Vec3;
typedef glm::mat4 Mat4;

class Camera;
class Scene {
  public:
    void AddCamera(Camera *c) { camera_ = c; }
    Camera *GetCamera() { return camera_; }
  private:
    Camera *camera_;
};

// global function...
Scene *GetWorld();

#endif // __LEARNOPENGL_LO_COMMON_H__
