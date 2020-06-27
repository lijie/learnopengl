#include "scene.h"

#include <stdio.h>

#include "renderer.h"

void Scene::Render(GlContext *ctx) {
  auto it = renderer_list_.begin();
  while (it != renderer_list_.end()) {
    // fprintf(stdout, "render object.\n");
    (*it)->Render(ctx);
    it++;
  }
  it = renderer_list1_.begin();
  while (it != renderer_list1_.end()) {
    // fprintf(stdout, "render object.\n");
    (*it)->Render(ctx);
    it++;
  }
  it = renderer_list2_.begin();
  while (it != renderer_list2_.end()) {
    // fprintf(stdout, "render object.\n");
    (*it)->Render(ctx);
    it++;
  }
}

void Scene::MoveTarget(int dir) {
  if (target_ == nullptr) return;

  target_->add_rotation(Vec3(0, 10, 0));
}

void Scene::AddRenderer(shared_ptr<Renderer> renderer, int pri) {
  if (pri == 0)
    renderer_list_.push_back(renderer);
  else if (pri == 1)
    renderer_list1_.push_back(renderer);
  else
    renderer_list2_.push_back(renderer);
}