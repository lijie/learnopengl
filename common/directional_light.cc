#include "directional_light.h"

#include "glm/gtx/string_cast.hpp"
#include "material.h"
#include "scene.h"

void DirectionalLight::SetUniforms(std::shared_ptr<Material> mat,
                                   const SceneCommonUniforms& common_uniforms) {
  char buf[64];

  Vec4 dir = glm::normalize(common_uniforms.ViewMatrix * Vec4(direction(), 1.0));
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

void LightManager::AddLight(LightPtr light) {
  if (light->light_type() == kDirectionalLight) {
    light->set_index(directional_light_index_);
    directional_light_index_++;
  } else if (light->light_type() == kPointLight) {
    light->set_index(point_light_index_);
    point_light_index_++;
  }

  light_vec_.push_back(light);
}

void LightManager::SetUniforms(MaterialPtr mat,
                               const SceneCommonUniforms& common_uniforms) {
  mat->DefineValue("DIRECTION_LIGHT_NUM", directional_light_index_);
  mat->DefineValue("POINT_LIGHT_NUM", point_light_index_);

  for (auto i = 0; i < light_vec_.size(); i++) {
    auto& light = light_vec_[i];
    light->SetUniforms(mat, common_uniforms);
  }
}
