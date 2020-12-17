#include "directional_light.h"

#include "glm/gtx/string_cast.hpp"
#include "material.h"
#include "scene.h"
#include "render_context.h"

void DirectionalLight::SetUniforms(std::shared_ptr<Material> mat,
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

void AmbientLight::SetUniforms(MaterialPtr mat,
                               const SceneCommonUniforms& common_uniforms) {
  mat->SetProperty("uAmbientLight", color());
}

void PointLight::SetUniforms(MaterialPtr mat,
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

void SpotLight::SetUniforms(MaterialPtr mat,
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
}

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
  } else if (light->light_type() == kPointLight) {
    light->set_index(point_light_index_);
    point_light_index_++;
  } else if (light->light_type() == kSpotLight) {
    light->set_index(spot_light_index_);
    spot_light_index_++;
  }

  light_vec_.push_back(light);
}

void LightManager::SetUniforms(MaterialPtr mat,
                               const SceneCommonUniforms& common_uniforms) {
  mat->DefineValue("DIRECTION_LIGHT_NUM", directional_light_index_);
  mat->DefineValue("POINT_LIGHT_NUM", point_light_index_);
  mat->DefineValue("SPOT_LIGHT_NUM", spot_light_index_);

  for (auto i = 0; i < light_vec_.size(); i++) {
    auto& light = light_vec_[i];
    light->SetUniforms(mat, common_uniforms);
  }
}
