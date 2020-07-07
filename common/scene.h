#ifndef  __LEARNOPENGL_COMMON_SCENE_H__
#define  __LEARNOPENGL_COMMON_SCENE_H__

#include <memory>
#include <list>
#include <functional>

#include "shape.h"

class Camera;
class Renderer;
class Transform;
class Framebuffer;

class Scene {
 public:
  void AddCamera(Camera *c) { camera_ = c; }
  Camera *GetCamera() { return camera_; }

  void AddRenderer(std::shared_ptr<Renderer> r, int pri = 0);
  void AddLightSource(std::shared_ptr<LightSource> ls) {
    light_source_ = ls;
    AddRenderer(ls);
  }

  std::shared_ptr<LightSource> GetLightSource() { return light_source_; }

  void SetTarget(std::shared_ptr<Transform> target) {target_ = target; }
  void MoveTarget(int dir);

  void ForEachRenderer(std::function<void(std::shared_ptr<Renderer>)> f) {
    auto it = renderer_list_.begin();
    while (it != renderer_list_.end()) {
      f(*it);
      it++;
    }
  }

  void Render(GlContext *ctx, std::shared_ptr<Framebuffer> target_buffer = nullptr);

 private:
  Camera *camera_;

  std::list<std::shared_ptr<Renderer>> renderer_list_;
  std::shared_ptr<LightSource> light_source_;
  std::shared_ptr<Transform> target_ = nullptr;

  void SortRenderer();
  void UpdateMaterialProperties(std::shared_ptr<Renderer> renderer);
};
#endif  // #define  __LEARNOPENGL_COMMON_SCENE_H__
