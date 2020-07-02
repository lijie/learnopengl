#include "scene.h"

#include <stdio.h>

#include <algorithm>

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "framebuffer.h"
#include "renderer.h"

class RendererCompare {
 public:
  bool operator()(const Renderer& r1, const Renderer& r2) const {
    return r1.priority() > r2.priority();
  }
};

void Scene::SortRenderer() {
  RendererCompare cmp;
  renderer_list_.sort([cmp](const std::shared_ptr<Renderer>& r1,
                            const std::shared_ptr<Renderer>& r2) {
    return cmp(*r1.get(), *r2.get());
  });
}

void Scene::Render(GlContext* ctx, std::shared_ptr<Framebuffer> target_buffer) {
  SortRenderer();

  if (target_buffer != nullptr) {
    target_buffer->Enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  }
  auto it = renderer_list_.begin();
  while (it != renderer_list_.end()) {
    // fprintf(stdout, "render object.\n");
    (*it)->Render(ctx);
    it++;
  }
  if (target_buffer != nullptr) {
    target_buffer->Disable();
  }
}

void Scene::MoveTarget(int dir) {
  if (target_ == nullptr) return;

  // target_->add_rotation(Vec3(0, 10, 0));
}

void Scene::AddRenderer(shared_ptr<Renderer> renderer, int pri) {
  renderer->set_proority(pri);
  renderer_list_.push_back(renderer);
}