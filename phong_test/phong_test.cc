#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "context.h"
#include "directional_light.h"
#include "glm/gtx/rotate_vector.hpp"
#include "material.h"
#include "model.h"
#include "scene.h"
#include "shader.h"
#include "shape.h"

// #define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static int screen_width = 1600;
static int screen_height = 1200;

// static Scene *world;

static double current_frame_time;
static double last_frame_time;

static bool first_mouse = true;

static double mouse_last_x = 0;
static double mouse_last_y = 0;

static bool enable_mouse_move = false;

static Mat4 light_source_model = Mat4(1.0);

static void framebuffer_size_callback(GLFWwindow *window, int width,
                                      int height) {
  glViewport(0, 0, width, height);
}

static void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (!enable_mouse_move) return;

  if (first_mouse) {
    first_mouse = false;
    mouse_last_x = xpos;
    mouse_last_y = ypos;
    return;
  }

  double delta_x = xpos - mouse_last_x;
  double delta_y = ypos - mouse_last_y;

  mouse_last_x = xpos;
  mouse_last_y = ypos;

  GetWorld()->GetCamera()->ProcessMouse(delta_x, delta_y);
}

static void mouse_button_callback(GLFWwindow *window, int button, int action,
                                  int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    enable_mouse_move = true;
  }
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    enable_mouse_move = false;
    first_mouse = true;
  }
}

void test_move(double dt) {
  // auto transform = GetWorld()->GetLightSource();
  // auto p = transform->position();

  // light_source_model = glm::translate(light_source_model, glm::vec3(-p.x, 0,
  // 0)); light_source_model = glm::rotate(light_source_model,
  // glm::radians(20.0f * (float)dt), glm::vec3(0.0f, 1.0f, 0.0f));
  // light_source_model = glm::translate(light_source_model, glm::vec3(p.x, 0,
  // 0));

  // transform->set_model(light_source_model);
}

static void process_input(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }

  double delta_time = current_frame_time - last_frame_time;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    GetWorld()->GetCamera()->ProcessKey(kCameraKeyForward, delta_time);
  } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    GetWorld()->GetCamera()->ProcessKey(kCameraKeyBackward, delta_time);
  } else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    GetWorld()->GetCamera()->ProcessKey(kCameraKeyLeft, delta_time);
  } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    GetWorld()->GetCamera()->ProcessKey(kCameraKeyRight, delta_time);
  } else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
    GetWorld()->MoveTarget(2);
    // test_move(delta_time);
  } else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
    GetWorld()->MoveTarget(3);
  }
}

// lighting
glm::vec3 light_position(1.2f, -1.0f, 2.0f);

glm::vec3 cube_albedo(1.0f, 1.0f, 1.0f);

static void init_cube_bottom(GlContext *c) {
  auto cube = make_shared<Cube>();
  auto mat = NewSharedObject<PhongMaterial>();

  MaterialParams params;
  params.Albedo = Vec3(0.8, 0.8, 0.8);
  params.Specular = Vec3(1.0, 1.0, 1.0);
  params.Shininess = 1.0f;
  // params.DiffuseTexture = "../texture/tmtjfd0r_2K_Albedo.jpg";
  // params.SpecularTexture = "../texture/tmtjfd0r_2K_Specular.jpg";
  mat->SetParams(params);

  cube->set_material(mat);
  cube->Translate(Vec3(0.0f, -10.0f, 0.0f));
  cube->Scale(Vec3(20.0f, 1, 20.0));
  GetWorld()->AddRenderer(cube);
  cube->set_display_name("cube1");
}
static void init_cube_top(GlContext *c) {
  auto cube = make_shared<Cube>();
  auto mat = NewSharedObject<PhongMaterial>();

  MaterialParams params;
  params.Albedo = Vec3(0.8, 0.8, 0.8);
  params.Specular = Vec3(1.0, 1.0, 1.0);
  params.Shininess = 1.0f;
  // params.DiffuseTexture = "../texture/tmtjfd0r_2K_Albedo.jpg";
  // params.SpecularTexture = "../texture/tmtjfd0r_2K_Specular.jpg";
  mat->SetParams(params);

  cube->set_material(mat);
  cube->Translate(Vec3(0.0f, 10.0f, 0.0f));
  cube->Scale(Vec3(20.0f, 1, 20.0));
  GetWorld()->AddRenderer(cube);
  cube->set_display_name("cube1");
}
static void init_cube_left(GlContext *c) {
  auto cube = make_shared<Cube>();
  auto mat = NewSharedObject<PhongMaterial>();

  MaterialParams params;
  params.Albedo = Vec3(1.0, 0.0, 0.0);
  params.Specular = Vec3(1.0, 1.0, 1.0);
  params.Shininess = 1.0f;
  // params.DiffuseTexture = "../texture/tmtjfd0r_2K_Albedo.jpg";
  // params.SpecularTexture = "../texture/tmtjfd0r_2K_Specular.jpg";
  mat->SetParams(params);

  cube->set_material(mat);
  cube->Translate(Vec3(-10.0f, 0.0f, 0.0f));
  cube->Scale(Vec3(1.0f, 20.0f, 20.0f));
  GetWorld()->AddRenderer(cube);
  cube->set_display_name("cube1");
}
static void init_cube_right(GlContext *c) {
  auto cube = make_shared<Cube>();
  auto mat = NewSharedObject<PhongMaterial>();

  MaterialParams params;
  params.Albedo = Vec3(0.0, 1.0, 0.0);
  params.Specular = Vec3(1.0, 1.0, 1.0);
  params.Shininess = 1.0f;
  // params.DiffuseTexture = "../texture/tmtjfd0r_2K_Albedo.jpg";
  // params.SpecularTexture = "../texture/tmtjfd0r_2K_Specular.jpg";
  mat->SetParams(params);

  cube->set_material(mat);
  cube->Translate(Vec3(10.0f, 0.0f, 0.0f));
  cube->Scale(Vec3(1.0f, 20.0f, 20.0f));
  GetWorld()->AddRenderer(cube);
  cube->set_display_name("cube1");
}
static void init_cube_back(GlContext *c) {
  auto cube = make_shared<Cube>();
  auto mat = NewSharedObject<PhongMaterial>();

  MaterialParams params;
  params.Albedo = Vec3(0.0, 0.0, 1.0);
  params.Specular = Vec3(1.0, 1.0, 1.0);
  params.Shininess = 1.0f;
  // params.DiffuseTexture = "../texture/tmtjfd0r_2K_Albedo.jpg";
  // params.SpecularTexture = "../texture/tmtjfd0r_2K_Specular.jpg";
  mat->SetParams(params);

  cube->set_material(mat);
  cube->Translate(Vec3(0.0f, 0.0f, -10.0f));
  cube->Scale(Vec3(20.0f, 20.0f, 1.0f));
  GetWorld()->AddRenderer(cube);
  cube->set_display_name("cube1");
}

static void init_cube2(GlContext *c) {
  auto cube = make_shared<Cube>();
  auto mat = NewSharedObject<UnlitMaterial>();

  MaterialParams params;
  params.Albedo = Vec3(0.6, 0.2, 0.2);
  mat->SetParams(params);

  cube->set_material(mat);
  cube->Translate(Vec3(-4.0f, 0.0f, 4.0f));
  cube->Scale(Vec3(2.0, 2.0, 2.0));
  GetWorld()->AddRenderer(cube);
  GetWorld()->SetTestShape(cube);
  cube->set_display_name("cube2");
}

static void init_model(GlContext *c) {
  int count = 0;
  int num = 1;
  for (int x = 0; x < num; x++) {
    for (int y = 0; y < num; y++) {
      for (int z = 0; z < num; z++) {
        auto model = NewSharedObject<Model>();
        std::string path = "../models/sphere.obj";
        // std::string path = "../models/nanosuit/nanosuit.obj";
        // std::string path = "../models/backpack/backpack.obj";
        model->Load(path, "", false, false);
        model->set_position(Vec3(x * 10.0f, y * 10.0f, z * 10.0f));
        // model->set_scale(Vec3(0.15));
        GetWorld()->AddRenderer(model);
        model->set_display_name("sphere");
        count++;
      }
    }
  }  
}

static PointLightPtr rotated_point_light = nullptr;
static void rotate_point_light() {
  if (rotated_point_light == nullptr)
    return;
  auto tramsform = rotated_point_light->GetTransform();
  auto direction = glm::rotate(tramsform->position(), float(glm::radians(2.0)),
                               Vec3(0, 1, 0));
  tramsform->set_position(direction);
}

static SpotLightPtr rotated_spot_light = nullptr;
static void rotate_spot_light() {
  if (rotated_spot_light == nullptr)
    return;
  auto tramsform = rotated_spot_light->GetTransform();
  auto direction = glm::rotate(tramsform->position(), float(glm::radians(2.0)),
                               Vec3(0, 1, 0));
  tramsform->set_position(direction);
}


static void init_scene(GlContext *c) {
  stbi_set_flip_vertically_on_load(1);

  Vec2 resolution(screen_width, screen_height);
  GetWorld()->SetResolution(resolution);

  Camera *camera = new Camera(Vec3(0.0, 0.0, 35.0), Vec3(0, 1, 0), Vec3(0, 0, 0), 45.0,
                              (double)screen_width / screen_height);
  GetWorld()->AddCamera(camera);

  auto directional_light =
      NewSharedObject<DirectionalLight>(Vec3(0.0f, 0.0f, 0.0f), COLOR_WHITE);
  directional_light->GetTransform()->set_position(Vec3(-5.0f, 5.0f, 0.0f));
  GetWorld()->AddLight(directional_light);

  auto ambient_light = NewSharedObject<AmbientLight>(Vec3(0.133, 0.133, 0.133));
  GetWorld()->AddLight(ambient_light);

  // auto point_light = NewSharedObject<PointLight>(0, 0, COLOR_WHITE);
  // point_light->GetTransform()->set_position(Vec3(7.0f, 0.0f, 0.0f));
  // GetWorld()->AddLight(point_light);
  // rotated_point_light = point_light;

  auto spot_light = NewSharedObject<SpotLight>(0, 0, COLOR_WHITE, glm::pi<float>() / 6.0f, 0.5);
  spot_light->GetTransform()->set_position(Vec3(0.0f, 15.0f, 0.0f));

  spot_light->set_target(Vec3(0.0f, 0.0f, 0.0f));

  ShadowParams shadow_params;
  shadow_params.Resolution = Vec2(1024.0f, 1024.0f);
  spot_light->EanbleShadow(shadow_params);

  GetWorld()->AddLight(spot_light);
  rotated_spot_light = spot_light;

  // auto spot_light_helper = NewSharedObject<LightHelper>(spot_light);
  // GetWorld()->AddRenderer(spot_light_helper);

  // init_cube_top(c);
  init_cube_bottom(c);
  init_cube_left(c);
  init_cube_right(c);
  init_cube_back(c);
  init_cube2(c);
  init_model(c);
}

static void draw(GlContext *c) {
  GetWorld()->Render(c);
  rotate_point_light();
  // rotate_spot_light();
}

static void release_resource(GlContext *c) {
  glDeleteVertexArrays(1, &c->vao);
  glDeleteBuffers(1, &c->vbo);
}

void glm_test() { glm::vec4 vec(1.0, 0.0, 0.0, 1.0); }

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);

  GLFWwindow *window =
      glfwCreateWindow(screen_width, screen_height, "OpenGL Demo", NULL, NULL);
  assert(window != NULL);
  glfwMakeContextCurrent(window);

  GlobalStart();
  GlContext *context = new GlContext();

  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  init_scene(context);

  // load_shader(context);
  // init_vertices(context);
  // init_texture(context);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);

  // glEnable(GL_BLEND);
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  int frame_count = 0;

  while (!glfwWindowShouldClose(window)) {
    last_frame_time = current_frame_time;
    current_frame_time = glfwGetTime();

    process_input(window);

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    draw(context);

    glfwSwapBuffers(window);
    glfwPollEvents();

    frame_count++;
    // if (frame_count > 1)
    //   break;
  }

  GlobalFinish();
  release_resource(context);
  delete context;
  glfwTerminate();
  return 0;
}
