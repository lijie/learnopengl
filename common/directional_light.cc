#include "directional_light.h"
#include "material.h"

void DirectionalLight::SetUniforms(std::shared_ptr<Material> mat) {
    mat->DefineValue("DIRECTION_LIGHT_NUM", 1);
    mat->SetProperty("directionalLights[0].direction", direction_);
    mat->SetProperty("directionalLights[0].color", color_);
}
