#include <stdio.h>
#include "scene.h"
#include "renderer.h"

void Scene::Render(GlContext *ctx) {
  auto it = renderer_list_.begin();
  while (it != renderer_list_.end()) {
    // fprintf(stdout, "render object.\n");
    (*it)->Render(ctx);
    it++;
  }
}
