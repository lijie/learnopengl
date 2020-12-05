#include "glad/glad.h"
#include "scene.h"

#include <GLFW/glfw3.h>
#include <stdio.h>

#include <algorithm>

#include "camera.h"
#include "directional_light.h"
#include "framebuffer.h"
#include "glm/gtx/string_cast.hpp"
#include "material.h"
#include "render_context.h"
#include "renderer.h"

class RendererCompare {
 public:
  bool operator()(const Renderer& r1, const Renderer& r2) const {
    return r1.priority() > r2.priority();
  }
};

Scene::Scene() { light_manager_ = new LightManager; }

Scene::~Scene() { delete light_manager_; }

void Scene::SortRenderer() {
  RendererCompare cmp;
  renderer_list_.sort([cmp](const std::shared_ptr<Renderer>& r1,
                            const std::shared_ptr<Renderer>& r2) {
    return cmp(*r1.get(), *r2.get());
  });
}

void Scene::UpdateMaterialProperties(RendererPtr renderer,
                                     MaterialPtr material,
                                     const RenderContext& context) {
  auto children = renderer->GetChildren();
  for (size_t i = 0; i < children.size(); i++) {
    auto child = children[i];
    auto mat = material == nullptr ? child->material() : material;
    UpdateMaterialProperties(child, mat, context);
  }

  glm::mat4 model = renderer->model();

  auto& view = context.CommonUniforms.ViewMatrix;
  // printf("glm view matrix: %s\n", glm::to_string(view).c_str());
  // printf("model matrix: %s\n", glm::to_string(model).c_str());
  auto& projection = context.CommonUniforms.ProjectionMatrix;
  auto mvp = projection * view * model;
  auto model_view = view * model;
  auto mv3x3 = glm::mat3(model_view);
  // printf("mv3x3 matrix: %s\n", glm::to_string(mv3x3).c_str());
  auto normal_model = glm::mat3(glm::transpose(glm::inverse(model)));
  auto normal_matrix = glm::transpose(glm::inverse(mv3x3));

  if (material == nullptr) return;

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

  // set light uniforms
  if (context.EnableLight) {
    light_manager_->SetUniforms(material, context.CommonUniforms);
  }

  // material->DefineValue("DIRECTION_LIGHT_NUM", 1);
  // material->SetProperty("directionalLights[0].direction", mv3x3 * Vec3(0.0,
  // -0.5, -0.5)); material->SetProperty("directionalLights[0].color",
  // Vec3(1.0, 1.0, 1.0));

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

void Scene::ShadowDepthMapPass() {
  RenderContext context;
  SceneCommonUniforms& common_unifroms = context.CommonUniforms;

  common_unifroms.ViewMatrix = GetCamera()->GetViewMatrix();
  common_unifroms.ProjectionMatrix = GetCamera()->GetProjectionMatrix();

  context.EnableLight = false;
  context.EnableShadow = false;

  if (depth_framebuffer_ == nullptr) {
    depth_framebuffer_ = NewSharedObject<DepthFramebuffer>(1024, 1024);
  }
  if (depthmap_material_ == nullptr) {
    depthmap_material_ = NewSharedObject<DepthMapMaterial>();
  }
  context.ShadowPassOutputFramebuffer = depth_framebuffer_;
  context.ContextMaterial = depthmap_material_;

  glViewport(0, 0, 1024, 1024);
  context.ShadowPassOutputFramebuffer->Enable();
  // TODO...
  glEnable(GL_DEPTH_TEST);
  glClear(GL_DEPTH_BUFFER_BIT);

  Draw(&context);
  context.ShadowPassOutputFramebuffer->Disable();
  glViewport(0, 0, 1600, 1200);

  if (!test_shape_->material()->HasProperty("uDiffuseTexture")) {
    auto test_texture = Texture::NewTextureWithTextureId(context.ShadowPassOutputFramebuffer->GetTextureId());
    test_shape_->material()->SetDiffuseTexture(test_texture);
    test_shape_->material()->DefineValue("DEPTH_TEXTURE", 1);
  }
}

void Scene::DrawPass() {
  RenderContext context;
  SceneCommonUniforms& common_unifroms = context.CommonUniforms;
  context.EnableLight = true;
  context.EnableShadow = true;
  context.ContextMaterial = nullptr;

  common_unifroms.ViewMatrix = GetCamera()->GetViewMatrix();
  common_unifroms.ProjectionMatrix = GetCamera()->GetProjectionMatrix();

  Draw(&context);
}

void Scene::Draw(RenderContext* context) {
  auto material = context->ContextMaterial;
  auto it = renderer_list_.begin();
  while (it != renderer_list_.end()) {
    // fprintf(stdout, "render object.\n");
    // if (!(*it)->visible()) {
    //   it++;
    //   continue;
    // }
    auto mat = material == nullptr ? (*it)->material() : material;
    UpdateMaterialProperties(*it, mat, *context);
    (*it)->Update(mat);
    it++;
  }
}

void Scene::Render(GlContext* ctx) {
  SortRenderer();
  ShadowDepthMapPass();
  DrawPass();
}

void Scene::MoveTarget(int dir) {
  if (target_ == nullptr) return;

  // target_->add_rotation(Vec3(0, 10, 0));
}

void Scene::AddRenderer(shared_ptr<Renderer> renderer, int pri) {
  renderer->set_proority(pri);
  renderer_list_.push_back(renderer);
}

void Scene::AddLight(LightPtr light) { light_manager_->AddLight(light); }
