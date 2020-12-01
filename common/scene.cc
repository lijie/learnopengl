#include "scene.h"

#include <stdio.h>

#include <algorithm>

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "framebuffer.h"
#include "renderer.h"
#include "camera.h"
#include "material.h"
#include "glm/gtx/string_cast.hpp"

#include "directional_light.h"

class RendererCompare {
 public:
  bool operator()(const Renderer& r1, const Renderer& r2) const {
    return r1.priority() > r2.priority();
  }
};

Scene::Scene() {
  light_manager_ = new LightManager;
}

Scene::~Scene() {
  delete light_manager_;
}

void Scene::SortRenderer() {
  RendererCompare cmp;
  renderer_list_.sort([cmp](const std::shared_ptr<Renderer>& r1,
                            const std::shared_ptr<Renderer>& r2) {
    return cmp(*r1.get(), *r2.get());
  });
}

void Scene::UpdateMaterialProperties(RendererPtr renderer, const SceneCommonUniforms& common_uniforms) {
  auto children = renderer->GetChildren();
  for (size_t i = 0; i < children.size(); i++) {
    auto child = children[i];
    UpdateMaterialProperties(child, common_uniforms);
  }

  glm::mat4 model = renderer->model();

  auto& view = common_uniforms.ViewMatrix;
  // printf("glm view matrix: %s\n", glm::to_string(view).c_str());
  // printf("model matrix: %s\n", glm::to_string(model).c_str());
  auto& projection = common_uniforms.ProjectionMatrix;
  auto mvp = projection * view * model;
  auto model_view = view * model;
  auto mv3x3 = glm::mat3(model_view);
  // printf("mv3x3 matrix: %s\n", glm::to_string(mv3x3).c_str());
  auto normal_model = glm::mat3(glm::transpose(glm::inverse(model)));
  auto normal_matrix = glm::transpose(glm::inverse(mv3x3));

  auto material = renderer->material();
  if (material == nullptr)
    return;

  // deprecated
  material->SetProperty("model", model);
  material->SetProperty("view", view);
  material->SetProperty("projection", projection);
  material->SetProperty("mvp", mvp);
  material->SetProperty("mv3x3", mv3x3);
  material->SetProperty("normal_model", normal_model);

  material->SetProperty("uModel", model);
  material->SetProperty("uView", view);
  material->SetProperty("uProjection", projection);
  material->SetProperty("uMVP", mvp);
  material->SetProperty("uMV3x3", mv3x3);
  material->SetProperty("uModelView", model_view);
  material->SetProperty("uNormalMatrix", normal_matrix);

  material->SetProperty("uSpecular", Vec3(1.0, 1.0, 1.0));
  material->SetProperty("uShininess", 8.0f);
  material->SetProperty("uAmbientLight", Vec3(0.3, 0.3, 0.3));

  // printf("view %s\n", glm::to_string(view).c_str());
  // printf("view %s\n", glm::to_string(glm::mat3(view)).c_str());

  // set light uniforms
  light_manager_->SetUniforms(material, common_uniforms);

  // material->DefineValue("DIRECTION_LIGHT_NUM", 1);
  // material->SetProperty("directionalLights[0].direction", mv3x3 * Vec3(0.0, -0.5, -0.5));
  // material->SetProperty("directionalLights[0].color", Vec3(1.0, 1.0, 1.0));

#if 0
  auto light_source = GetLightSource();
  if (light_source != nullptr) {
    auto light_position = light_source->position();
    material->SetProperty("light_position", light_position);

    auto light_power = light_source->power();
    material->SetProperty("light_power", light_power);

    auto light_color = light_source->color();
    material->SetProperty("light_color", light_color);
  }
#endif

  auto camera_position = GetWorld()->GetCamera()->position();
  material->SetProperty("camera_position", camera_position);
}

void Scene::Render(GlContext* ctx, std::shared_ptr<Framebuffer> target_buffer) {
  SortRenderer();

  if (target_buffer != nullptr) {
    target_buffer->Enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  }

  SceneCommonUniforms common_unifroms;

  common_unifroms.ViewMatrix = GetCamera()->GetViewMatrix();
  common_unifroms.ProjectionMatrix = GetCamera()->GetProjectionMatrix();

  auto it = renderer_list_.begin();
  while (it != renderer_list_.end()) {
    // fprintf(stdout, "render object.\n");
    UpdateMaterialProperties(*it, common_unifroms);
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

void Scene::AddLight(LightPtr light) {
  light_manager_->AddLight(light);
}
