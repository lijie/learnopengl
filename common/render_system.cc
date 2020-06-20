#include "lo_common.h"

class RenderSystem {
 public:
  RenderSystem(GLContext *ctx): ctx_(ctx) {};
  void Update();

 private:
  GLContext *ctx_;
};

void RenderSystem::Update() {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  auto world = GetWorld();

  world->ForEachRenderer(
      [](renderer_t r) {
        r->Draw(ctx_);
      });
}

