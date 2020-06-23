#ifndef  __LEARNOPENGL_COMMON_SCENE_H__
#define  __LEARNOPENGL_COMMON_SCENE_H__

#include <memory>
#include <list>
#include <functional>

#include "shape.h"

class Camera;
class Renderer;

class Scene {
 public:
  void AddCamera(Camera *c) { camera_ = c; }
  Camera *GetCamera() { return camera_; }

  void AddRenderer(std::shared_ptr<Renderer> r) { renderer_list_.push_back(r); }

  void AddLightSource(std::shared_ptr<LightSource> ls) {
    light_source_ = ls;
    AddRenderer(ls);
  }

  void ForEachRenderer(std::function<void(std::shared_ptr<Renderer>)> f) {
    auto it = renderer_list_.begin();
    while (it != renderer_list_.end()) {
      f(*it);
      it++;
    }
  }

  void Render(GlContext *ctx);

 private:
  Camera *camera_;

  std::list<std::shared_ptr<Renderer>> renderer_list_;
  std::shared_ptr<LightSource> light_source_;
};
#endif  // #define  __LEARNOPENGL_COMMON_SCENE_H__
