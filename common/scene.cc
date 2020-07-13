#include "scene.h"

#include <stdio.h>

#include <algorithm>

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "framebuffer.h"
#include "renderer.h"
#include "camera.h"
#include "material.h"

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

void Scene::UpdateMaterialProperties(std::shared_ptr<Renderer> renderer) {
  auto children = renderer->GetChildren();
  for (size_t i = 0; i < children.size(); i++) {
    auto child = children[i];
    UpdateMaterialProperties(child);
  }

  glm::mat4 model = renderer->model();

  auto view = GetCamera()->GetViewMatrix();
  auto projection = GetCamera()->GetProjectionMatrix();
  auto mvp = projection * view * model;
  auto mv3x3 = glm::mat3(view * model);
  auto normal_model = glm::mat3(glm::transpose(glm::inverse(model)));

  auto material = renderer->material();
  if (material == nullptr)
    return;

  material->SetProperty("model", model);
  material->SetProperty("view", view);
  material->SetProperty("projection", projection);
  material->SetProperty("mvp", mvp);
  material->SetProperty("mv3x3", mv3x3);
  material->SetProperty("normal_model", normal_model);

  auto light_source = GetLightSource();
  if (light_source != nullptr) {
    auto light_position = light_source->position();
    material->SetProperty("light_position", light_position);

    auto light_power = light_source->power();
    material->SetProperty("light_power", light_power);

    auto light_color = light_source->color();
    material->SetProperty("light_color", light_color);
  }

  auto camera_position = GetWorld()->GetCamera()->position();
  material->SetProperty("camera_position", camera_position);
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
    UpdateMaterialProperties(*it);
    (*it)->Update(0);
    it++;
  }
  if (target_buffer != nullptr) {
    target_buffer->Blit();
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
