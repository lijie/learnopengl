#ifndef __LEARNOPENGL_LO_COMMON_H__
#define __LEARNOPENGL_LO_COMMON_H__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <list>
#include <memory>

typedef glm::vec2 Vec2;
typedef glm::vec3 Vec3;
typedef glm::vec4 Vec4;
typedef glm::mat3 Mat3;
typedef glm::mat4 Mat4;

using std::shared_ptr;
using std::make_shared;

class Scene;

// global function...
Scene *GetWorld();
void GlobalStart();
void GlobalFinish();

void MaterialStart();
void MaterialFinish();

#endif  // __LEARNOPENGL_LO_COMMON_H__
