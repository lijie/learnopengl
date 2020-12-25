#include "directional_light.h"

#include "glm/gtx/string_cast.hpp"
#include "material.h"
#include "scene.h"
#include "render_context.h"
#include "framebuffer.h"

RenderContext* Light::AllocAndInitDepthMapContext() {
  auto ctx = new RenderContext;
  ctx->ContextMaterial = NewSharedObject<DepthMapMaterial>();
#ifdef PACK_DEPTH
  ctx->OutputFramebuffer = NewSharedObject<TextureFramebuffer>(
      shadow_params_.Resolution.x, shadow_params_.Resolution.y);
#else
  ctx->OutputFramebuffer = NewSharedObject<DepthFramebuffer>(
      shadow_params_.Resolution.x, shadow_params_.Resolution.y);
#endif

  return ctx;
}

void DirectionalLight::SetUniforms(const RenderContext& ctx, std::shared_ptr<Material> mat,
                                   const SceneCommonUniforms& common_uniforms) {
  char buf[64];

  Vec4 dir = glm::normalize(common_uniforms.ViewMatrix * Vec4(direction(), 0.0));
  // printf("dl position: %s\n", glm::to_string(transform_.position()).c_str());
  // printf("dl target: %s\n", glm::to_string(target_).c_str());
  // printf("dl in view space: %s\n", glm::to_string(dir).c_str());

  snprintf(buf, sizeof(buf), "directionalLights[%d].direction", index());
  mat->SetProperty(std::string(buf), Vec3(dir));
  snprintf(buf, sizeof(buf), "directionalLights[%d].color", index());
  mat->SetProperty(std::string(buf), color_);
}

void AmbientLight::SetUniforms(const RenderContext& ctx, MaterialPtr mat,
                               const SceneCommonUniforms& common_uniforms) {
  mat->SetProperty("uAmbientLight", color());
}

void PointLight::SetUniforms(const RenderContext& ctx, MaterialPtr mat,
                             const SceneCommonUniforms& common_uniforms) {
  char buf[64];

  // struct PointLight {
  //       vec3 position;
  //       vec3 color;
  //       float distance;
  //       float decay;
  //   };

  Vec4 view_position = common_uniforms.ViewMatrix * Vec4(transform_.position(), 1.0);

  snprintf(buf, sizeof(buf), "pointLights[%d].position", index());
  mat->SetProperty(std::string(buf), Vec3(view_position));
  snprintf(buf, sizeof(buf), "pointLights[%d].color", index());
  mat->SetProperty(std::string(buf), color_);
  snprintf(buf, sizeof(buf), "pointLights[%d].distance", index());
  mat->SetProperty(std::string(buf), distance_);
  snprintf(buf, sizeof(buf), "pointLights[%d].decay", index());
  mat->SetProperty(std::string(buf), decay_);
}

void SpotLight::SetUniforms(const RenderContext& ctx, MaterialPtr mat,
                            const SceneCommonUniforms& common_uniforms) {
  // struct SpotLight {
  //     vec3 position;
  //     vec3 direction;
  //     vec3 color;
  //     float distance;
  //     float decay;
  //     float coneCos;
  //     float penumbraCos;
  // };

  if (ctx.Pass != kRenderPassDraw)
    return;

  char buf[64];

  Vec4 view_position =
      common_uniforms.ViewMatrix * Vec4(transform_.position(), 1.0);
  Vec4 view_direction = glm::normalize(common_uniforms.ViewMatrix * Vec4(direction(), 0.0));
  float cone_cos = glm::cos(cone_);
  float penumbra_cos = glm::cos(cone_ * (1 - penumbra_));  // glm::cos(penumbra_);

  // printf("SpotLight: cone_cos[%f], penumbra_cos[%f].\n", cone_cos, penumbra_cos);
  // printf("SpotLight: ViewDirection: %s\n", glm::to_string(view_direction).c_str());
  // printf("SpotLight: ViewDirection: %s\n", glm::to_string(glm::normalize(direction())).c_str());
  // printf("SpotLight: target: %s\n", glm::to_string(target_).c_str());
  // printf("SpotLight: position: %s\n", glm::to_string(transform_.position()).c_str());

  // Vec4 target_view = common_uniforms.ViewMatrix * Vec4(target_, 1.0);
  // Vec4 position_view = common_uniforms.ViewMatrix * Vec4(transform_.position(), 1.0);
  // printf("SpotLight: target1: %s\n", glm::to_string(target_view).c_str());
  // printf("SpotLight: position1: %s\n", glm::to_string(position_view).c_str());

  snprintf(buf, sizeof(buf), "spotLights[%d].position", index());
  mat->SetProperty(std::string(buf), Vec3(view_position));
  snprintf(buf, sizeof(buf), "spotLights[%d].direction", index());
  mat->SetProperty(std::string(buf), Vec3(view_direction));
  snprintf(buf, sizeof(buf), "spotLights[%d].color", index());
  mat->SetProperty(std::string(buf), color_);
  snprintf(buf, sizeof(buf), "spotLights[%d].distance", index());
  mat->SetProperty(std::string(buf), distance_);
  snprintf(buf, sizeof(buf), "spotLights[%d].decay", index());
  mat->SetProperty(std::string(buf), decay_);
  snprintf(buf, sizeof(buf), "spotLights[%d].coneCos", index());
  mat->SetProperty(std::string(buf), cone_cos);
  snprintf(buf, sizeof(buf), "spotLights[%d].penumbraCos", index());
  mat->SetProperty(std::string(buf), penumbra_cos);

  if (enable_shadow_) {
    // struct SpotLightShadow {
    // float shadowBias;
    // float shadowNormalBias;
    // float shadowRadius;
    //   vec2 shadowMapSize;
    // }
    // SpotLightShadow spotLightShadow[SPOT_SHADOW_NUM];
    snprintf(buf, sizeof(buf), "spotLightShadow[%d].shadowBias", shadow_index());
    mat->SetProperty(std::string(buf), 0.0f);
    snprintf(buf, sizeof(buf), "spotLightShadow[%d].shadowNormalBias", shadow_index());
    mat->SetProperty(std::string(buf), 0.0f);
    snprintf(buf, sizeof(buf), "spotLightShadow[%d].shadowRadius", shadow_index());
    mat->SetProperty(std::string(buf), 0.0f);
    snprintf(buf, sizeof(buf), "spotLightShadow[%d].shadowMapSize", shadow_index());
    mat->SetProperty(std::string(buf), Vec2(0.0f, 0.0f));

    // printf("LightSpaceMatrix: %s\n", glm::to_string(light_space_mat_).c_str());
    snprintf(buf, sizeof(buf), "spotShadowMatrix[%d]", shadow_index());
    mat->SetProperty(std::string(buf), light_space_mat_);

    if (depthmap_ctx_ && depthmap_ctx_->OutputFramebuffer != nullptr) {
      // shadowmap
      auto depth_texture = Texture::NewTextureWithTextureId(depthmap_ctx_->OutputFramebuffer->GetTextureId());
      snprintf(buf, sizeof(buf), "spotShadowMap[%d]", shadow_index());
      mat->SetProperty(std::string(buf), depth_texture);
    }
  }
}

RenderContext* SpotLight::GetDepthMapContext(RenderPass pass) {
  if (depthmap_ctx_ == nullptr) {
    printf("SpotLight::GetDepthMapContext\n");
    depthmap_ctx_ = AllocAndInitDepthMapContext();

    auto size = shadow_params_.Resolution;
    // depthmap_ctx_->ContextMaterial = NewSharedObject<DepthMapMaterial>();
    // depthmap_ctx_->OutputFramebuffer =
    //     NewSharedObject<TextureFramebuffer>(size.x, size.y);
    depthmap_ctx_->EnableLight = false;
    depthmap_ctx_->EnableShadow = false;
    depthmap_ctx_->GLEnableFlags.push_back(GL_DEPTH_TEST);
    depthmap_ctx_->GLClearFlags =
        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
    depthmap_ctx_->Resolution = size;
  }

  auto position = GetTransform()->position();
  auto target = target_;
  auto direction = glm::normalize(target - position);
  auto up = Vec3(0, 1, 0);

  // printf("ViewMat: %f\n", glm::abs(glm::dot(direction, up)));
  if (glm::abs(glm::dot(direction, up)) > 0.95) {
    up = Vec3(0.0, 0.5, -0.5);
  }

  auto view_mat = glm::lookAt(position, target, up);
  // printf("ViewMat: %s\n", glm::to_string(view_mat).c_str());
  auto proj_mat = glm::perspective(glm::radians(45.0f), 1.0f / 1.0f, 0.1f, 100.0f);

  depthmap_ctx_->CommonUniforms.ProjectionMatrix = proj_mat;
  depthmap_ctx_->CommonUniforms.ViewMatrix = view_mat;

  light_space_mat_ = proj_mat * view_mat;

  // printf("0 at ViewSpace: %s\n", glm::to_string(light_space_mat_ * Vec4(0, 0, 0, 1.0)).c_str());

  return depthmap_ctx_;
}

#if 0
void SpotLight::RenderShadowMap(RenderSceneFunc render) {
  // prepare data

  // render
  render();
}
#endif

LightHelper::LightHelper(LightPtr light) {
  light_ = light;

  auto cube = make_shared<Cube>();
  auto mat = NewSharedObject<UnlitMaterial>();
  MaterialParams params;
  params.Albedo = Vec3(1.0, 1.0, 1.0);
  params.Specular = Vec3(1.0, 1.0, 1.0);
  params.Shininess = 1.0f;
  mat->SetParams(params);

  cube->set_material(mat);
  cube->Translate(light->GetTransform()->position());
  cube->Scale(Vec3(0.5f, 0.5, 0.5f));

  GetWorld()->AddRenderer(cube);

  shape_ = cube;
}

void LightHelper::BeforeUpdate() {
  shape_->Translate(light_->GetTransform()->position());
}

void LightManager::AddLight(LightPtr light) {
  if (light->light_type() == kDirectionalLight) {
    light->set_index(directional_light_index_);
    directional_light_index_++;
    if (light->IsShadowEnabled()) {
      light->set_shadow_index(directional_shadow_index_);
      directional_shadow_index_++;
    }
  } else if (light->light_type() == kPointLight) {
    light->set_index(point_light_index_);
    point_light_index_++;
    if (light->IsShadowEnabled()) {
      light->set_shadow_index(point_shadow_index_);
      point_shadow_index_++;
    }
  } else if (light->light_type() == kSpotLight) {
    light->set_index(spot_light_index_);
    spot_light_index_++;
    if (light->IsShadowEnabled()) {
      light->set_shadow_index(spot_shadow_index_);
      spot_shadow_index_++;
    }
  }

  light_vec_.push_back(light);
}

void LightManager::SetUniforms(const RenderContext& ctx, MaterialPtr mat,
                               const SceneCommonUniforms& common_uniforms) {
  mat->DefineValue("DIRECTION_LIGHT_NUM", directional_light_index_);
  mat->DefineValue("POINT_LIGHT_NUM", point_light_index_);
  mat->DefineValue("SPOT_LIGHT_NUM", spot_light_index_);

  mat->DefineValue("DIRECTION_SHADOW_NUM", directional_shadow_index_);
  mat->DefineValue("POINT_SHADOW_NUM", point_shadow_index_);
  mat->DefineValue("SPOT_SHADOW_NUM", spot_shadow_index_);

  for (auto i = 0; i < light_vec_.size(); i++) {
    auto& light = light_vec_[i];
    light->SetUniforms(ctx, mat, common_uniforms);
  }
}

void LightManager::ForEachLight(std::function<void(LightPtr)> cb) {
  for (auto i = 0; i < light_vec_.size(); i++) {
    auto& light = light_vec_[i];
    cb(light);
  }
}