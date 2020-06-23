#ifndef __LEARNOPENGL_LO_COMMON_H__
#define __LEARNOPENGL_LO_COMMON_H__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <list>
#include <memory>

typedef glm::vec2 Vec2;
typedef glm::vec3 Vec3;
typedef glm::mat4 Mat4;

class Scene;

// global function...
Scene *GetWorld();

#endif  // __LEARNOPENGL_LO_COMMON_H__
