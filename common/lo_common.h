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

template<typename T, typename ...Ts>
std::shared_ptr<T> NewSharedObject(Ts ...args) {
  return std::make_shared<T>(args...);
}

class Material;
class Texture;
class Shader;
class Renderer;
class Light;
class Shape;

typedef std::shared_ptr<Material> MaterialPtr;
typedef std::shared_ptr<Texture> TexturePtr;
typedef std::shared_ptr<Shader> ShaderPtr;
typedef std::shared_ptr<Renderer> RendererPtr;
typedef std::shared_ptr<Light> LightPtr;
typedef std::shared_ptr<Shape> ShapePtr;

#define COLOR_WHITE Vec3(1.0f, 1.0f, 1.0f)
#define COLOR_BLACK Vec3(0.0f, 0.0f, 0.0f)

extern int glCheckError(const char *file, int line);
#if 1
#define CHECK_GL_ERROR  glCheckError(__FILE__, __LINE__)
#else
#define CHECK_GL_ERROR
#endif

#endif  // __LEARNOPENGL_LO_COMMON_H__
