#include "directional_light.h"

#include "glm/gtx/string_cast.hpp"
#include "material.h"
#include "scene.h"

void DirectionalLight::SetUniforms(std::shared_ptr<Material> mat,
                                   const SceneCommonUniforms& common_uniforms) {
  char buf[64];

  Vec4 dir = common_uniforms.ViewMatrix * Vec4(direction(), 1.0);
  // printf("dir: %s\n", glm::to_string(dir));

  snprintf(buf, sizeof(buf), "directionalLights[%d].direction", index());
  mat->SetProperty(std::string(buf), Vec3(dir));
  snprintf(buf, sizeof(buf), "directionalLights[%d].color", index());
  mat->SetProperty(std::string(buf), color_);
}

void LightManager::AddLight(LightPtr light) {
  if (light->light_type() == kDirectionalLight) {
    light->set_index(directional_light_index_);
    directional_light_index_++;
  }

  light_vec_.push_back(light);
}

void LightManager::SetUniforms(MaterialPtr mat,
                               const SceneCommonUniforms& common_uniforms) {
  mat->DefineValue("DIRECTION_LIGHT_NUM", directional_light_index_);

  for (auto i = 0; i < light_vec_.size(); i++) {
    auto& light = light_vec_[i];
    light->SetUniforms(mat, common_uniforms);
  }
}
