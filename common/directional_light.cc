#include "directional_light.h"
#include "material.h"

void DirectionalLight::set_rotation(float angle, Vec3 axis) {
    Mat4 model = Mat3(1.0);
    model = glm::rotate(model, glm::radians(angle), axis);
    Vec4 new_direction = model * Vec4(direction_, 1.0);
    set_direction(Vec3(new_direction));
}

void DirectionalLight::SetUniforms(std::shared_ptr<Material> mat) {
    mat->DefineValue("DIRECTION_LIGHT_NUM", 1);
    mat->SetProperty("directionalLights[0].direction", direction_);
    mat->SetProperty("directionalLights[0].color", color_);
}

void LightManager::AddLight(std::shared_ptr<Light> light) {

}
