#include <map>
#include <string>
#include "material.h"

static std::map<std::string, material_t> MaterialCollections;
static std::map<std::string, shader_t> ShaderCollections;

void Material::Init() {
}

material_t Material::FindMaterial(const std::string& name) {
  auto it = MaterialCollections.find(name);
  if (it == MaterialCollections.end())
    return nullptr;
  return it.second();
}
