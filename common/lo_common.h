#ifndef __LEARNOPENGL_LO_COMMON_H__
#define __LEARNOPENGL_LO_COMMON_H__

#include <list>
#include <memory>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Renderer;

typedef glm::vec3 Vec3;
typedef glm::mat4 Mat4;

typedef std::shared_ptr<Renderer> renderer_t;

class Camera;
class Scene {
 public:
  void AddCamera(Camera *c) { camera_ = c; }
  Camera *GetCamera() { return camera_; }

  void AddRenderer(renderer_t r) {
    renderer_list_.push_back(r);
  }

  void ForEachRenderer(std::function<void(renderer_t)> f) {
    auto it = renderer_list_.begin();
    while (it != renderer_list_.end()) {
      f(*it);
      it++;
    }
  }
 private:
  Camera *camera_;

  std::list<renderer_t> renderer_list_;
};

// global function...
Scene *GetWorld();

#endif // __LEARNOPENGL_LO_COMMON_H__
