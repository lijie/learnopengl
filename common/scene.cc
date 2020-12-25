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

Scene::Scene() {
  light_manager_ = new LightManager;
  resolution_ = Vec2(0.0f, 0.0f);
  current_resolution_ = resolution_;
  
}

Scene::~Scene() {
  if (default_context_ != nullptr) delete default_context_;
  if (depthmap_context_ != nullptr) delete depthmap_context_;
  delete light_manager_;
}

RenderContext* Scene::InitDefaultContext() {
  if (default_context_ != nullptr) return default_context_;
  auto ctx = new RenderContext;
  // ctx->Camera = camera_;
  ctx->Pass = kRenderPassDraw;
  ctx->ContextMaterial = nullptr;
  ctx->OutputFramebuffer = nullptr;
  ctx->EnableLight = true;
  ctx->EnableShadow = true;
  ctx->GLEnableFlags.push_back(GL_DEPTH_TEST);
  ctx->GLEnableFlags.push_back(GL_MULTISAMPLE);
  ctx->GLClearFlags =
      GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
  ctx->Resolution = resolution_;
  default_context_ = ctx;
  return ctx;
}

RenderContext* Scene::InitDepthMapContext() {
  if (depthmap_context_ != nullptr) return depthmap_context_;
  Vec2 size = Vec2(1024, 1024);
  auto ctx = new RenderContext;
  // ctx->Camera = camera_;
  ctx->Pass = kRenderPassDepthMap;
  ctx->ContextMaterial = NewSharedObject<DepthMapMaterial>();
  ctx->OutputFramebuffer = NewSharedObject<TextureFramebuffer>(size.x, size.y);
  ctx->EnableLight = false;
  ctx->EnableShadow = false;
  ctx->GLEnableFlags.push_back(GL_DEPTH_TEST);
  ctx->GLClearFlags =
      GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
  ctx->Resolution = size;
  depthmap_context_ = ctx;
  return ctx;
}

void Scene::SetResolution(const Vec2& size) {
  if (resolution_.x == 0.0f && resolution_.y == 0.0f) {
    resolution_ = size;
    current_resolution_ = resolution_;
  } else {
    current_resolution_ = size;
  }
  glViewport(0, 0, size.x, size.y);
}

void Scene::RestoreResolution() {
  current_resolution_ = resolution_;
  glViewport(0, 0, current_resolution_.x, current_resolution_.y);
}

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

  // printf("0 at CameraSpace: %s\n", glm::to_string(projection * view * Vec4(0, 0, 0, 1.0)).c_str());

  // set light uniforms
  if (context.EnableLight) {
    light_manager_->SetUniforms(context, material, context.CommonUniforms);
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

#if 0
void Scene::RenderDepthMapForAllLights() {
  auto save_camera = GetCamera();
  auto depth_camera =
      new Camera(Vec3(0.0, 0.0, 35.0), Vec3(0, 1, 0), Vec3(0, 0, 0), 45.0,
                 resolution_.x / resolution_.y);
  AddCamera(depth_camera);

  auto render_cb = [this](LightPtr light) {
    if (!light->IsShadowEnabled()) return;
    const ShadowParams& params = light->GetShadowParams();
    auto output = light->GetShadowMapOutputFramebuffer();
    if (output == nullptr) return;
    // update camera
    GetCamera()->set_position(light->GetTransform()->position());
    GetCamera()->set_target();
    this->RenderDepthMap(params.Resolution, output);
  };

  light_manager_->ForEachLight(render_cb);
}
#endif

void Scene::RenderSceneWithContext(RenderContext *context) {
  // auto save_camera = GetCamera();
  // if (save_camera != context->Camera) {
  //   AddCamera(context->Camera);
  // }

  if (context->Resolution != resolution_) {
    SetResolution(context->Resolution);
  }

  if (context->OutputFramebuffer != nullptr) {
    context->OutputFramebuffer->Enable();
  }

  glClear(context->GLClearFlags);
  // TODO(): 不是每次渲染都需要
  for (auto i = 0; i < context->GLEnableFlags.size(); i++) {
    glEnable(context->GLEnableFlags[i]);
  }

  Draw(context);

  if (context->OutputFramebuffer != nullptr) {
    context->OutputFramebuffer->Disable();
  }

  RestoreResolution();

  // restore camera
  // if (save_camera != GetCamera()) {
  //   AddCamera(save_camera);
  // }
}

#if 0
// render current scene depth map to output buffer
void RenderDepthMap(const Vec2& size, FramebufferPtr output) {
  RenderContext context;
  SceneCommonUniforms& common_unifroms = context.CommonUniforms;

  common_unifroms.ViewMatrix = GetCamera()->GetViewMatrix();
  common_unifroms.ProjectionMatrix = GetCamera()->GetProjectionMatrix();

  context.EnableLight = false;
  context.EnableShadow = false;

  if (depthmap_material_ == nullptr) {
    depthmap_material_ = NewSharedObject<DepthMapMaterial>();
  }
  context.ContextMaterial = depthmap_material_;

  glViewport(0, 0, size.x, size.y);
  output->Enable();
  // TODO...
  glEnable(GL_DEPTH_TEST);
  glClear(GL_DEPTH_BUFFER_BIT);

  Draw(&context);
  output->Disable();
  glViewport(0, 0, resolution_.x, resolution_.y);
}
#endif

void Scene::ShadowDepthMapPass() {
#if 0
  auto ctx = InitDepthMapContext();
  ctx->CommonUniforms.ViewMatrix = GetCamera()->GetViewMatrix();
  ctx->CommonUniforms.ProjectionMatrix = GetCamera()->GetProjectionMatrix();
  RenderSceneWithContext(ctx);

  if (!test_shape_->material()->HasProperty("uDiffuseTexture")) {
    auto test_texture = Texture::NewTextureWithTextureId(depthmap_context_->OutputFramebuffer->GetTextureId());
    test_shape_->material()->SetDiffuseTexture(test_texture);
    test_shape_->material()->DefineValue("DEPTH_TEXTURE", 1);
  }
#else
  auto render_cb = [this](LightPtr light) {
    if (!light->IsShadowEnabled()) return;
    auto ctx = light->GetDepthMapContext(kRenderPassDepthMap);
    // ctx->CommonUniforms.ViewMatrix = GetCamera()->GetViewMatrix();
    // ctx->CommonUniforms.ProjectionMatrix = GetCamera()->GetProjectionMatrix();
    this->RenderSceneWithContext(ctx);

    if (!test_shape_->material()->HasProperty("uDiffuseTexture")) {
      auto test_texture = Texture::NewTextureWithTextureId(ctx->OutputFramebuffer->GetTextureId());
      test_shape_->material()->SetDiffuseTexture(test_texture);
      test_shape_->material()->DefineValue("USE_DEPTH_TEXTURE", 1);
    }
  };

  light_manager_->ForEachLight(render_cb);
#endif
}

void Scene::DrawPass() {
  auto ctx = InitDefaultContext();
  ctx->CommonUniforms.ViewMatrix = GetCamera()->GetViewMatrix();
  ctx->CommonUniforms.ProjectionMatrix = GetCamera()->GetProjectionMatrix();
  RenderSceneWithContext(ctx);
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
