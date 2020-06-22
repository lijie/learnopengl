#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "context.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "shader.h"
#include "material.h"
#include "camera.h"
#include "shape.h"
#include "model.h"

// #define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static int screen_width = 1600;
static int screen_height = 1200;

static Scene *world;

static double current_frame_time;
static double last_frame_time;

static bool first_mouse = true;

static double mouse_last_x = 0;
static double mouse_last_y = 0;

static bool enable_mouse_move = false;

struct image_data {
  uint8_t *data;
  int width;
  int height;
  int nr_channel;
};

class LightCube : public Cube {
 public:
  LightCube();
};

LightCube::LightCube() {

}

class SimpleCube : public Cube {

};

static void framebuffer_size_callback(GLFWwindow *window, int width,
                                      int height) {
  glViewport(0, 0, width, height);
}

static void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (!enable_mouse_move)
    return;

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

  world->GetCamera()->ProcessMouse(delta_x, delta_y);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
      enable_mouse_move = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
      enable_mouse_move = false;
      first_mouse = true;
    }
}

static void process_input(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }

  double delta_time = current_frame_time - last_frame_time;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    world->GetCamera()->ProcessKey(kCameraKeyForward, delta_time);
  } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    world->GetCamera()->ProcessKey(kCameraKeyBackward, delta_time);
  } else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    world->GetCamera()->ProcessKey(kCameraKeyLeft, delta_time);
  } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    world->GetCamera()->ProcessKey(kCameraKeyRight, delta_time);
  }
}

// cube vertices and normal
float cube_vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

glm::vec3 cube_positions[] = {
    glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
    glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};


static Material light_material;
static Material cube_material;
// static Shader light_shader;
// static Shader cube_shader;

// lighting
glm::vec3 light_position(1.2f, -1.0f, 2.0f);

glm::vec3 cube_albedo(1.0f, 1.0f, 1.0f);

static void init_scene() {
  world = new Scene();
  Camera *camera = new Camera(Vec3(0.0, 0.0, 3.0), Vec3(0, 1, 0), 45.0, (double)screen_width / screen_height);
  world->AddCamera(camera);
}

static void draw_cube(GlContext *c) {
  cube_material.shader->Use();
  glm::mat4 model = glm::mat4(1.0);
  model = glm::translate(model, light_position);
  model = glm::scale(model, glm::vec3(0.2f));  // a smaller cube
  // model = glm::scale(model, glm::vec3(1, 1, 1));
  // model = glm::rotate(model, glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f,
  // 0.5f)); model = glm::translate(model, cube_positions[i]);

  unsigned int loc = cube_material.shader->GetUniformLocation("model");
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model));

  loc = cube_material.shader->GetUniformLocation("view");
  auto view = world->GetCamera()->GetViewMatrix();
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view));

  loc = cube_material.shader->GetUniformLocation("projection");
  auto projection = world->GetCamera()->GetProjectionMatrix();
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection));

  glDrawArrays(GL_TRIANGLES, 0, 36);
}

static void draw_light(GlContext *c) {
  light_material.shader->Use();
  auto model = glm::mat4(1.0f);

  unsigned int loc = light_material.shader->GetUniformLocation("model");
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model));

  loc = light_material.shader->GetUniformLocation("view");
  auto view = world->GetCamera()->GetViewMatrix();
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view));

  loc = light_material.shader->GetUniformLocation("projection");
  auto projection = world->GetCamera()->GetProjectionMatrix();
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection));

  loc = light_material.shader->GetUniformLocation("lightPos");
  glUniform3fv(loc, 1, glm::value_ptr(light_position));

  loc = light_material.shader->GetUniformLocation("albedo");
  glUniform3fv(loc, 1, glm::value_ptr(cube_albedo));

  // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, light_material.texture_id[0]);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, light_material.texture_id[1]);

  glDrawArrays(GL_TRIANGLES, 0, 36);
}

static void draw(GlContext *c) {
  draw_light(c);
  draw_cube(c);  
}

static void init_vertices(GlContext *c) {
  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices,
               GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // normal
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // texture coord
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  c->vao = VAO;
  c->vbo = VBO;
}

static void load_shader(GlContext *c) {
  light_material.shader = std::make_shared<Shader>();
  cube_material.shader = std::make_shared<Shader>();

    if (!light_material.shader->Open("../basic_lighting/basic_lighting_v.glsl", "../basic_lighting/basic_lighting_f.glsl")) {
        assert(0);
    }
    if (!light_material.shader->CompileAndLink()) {
        assert(0);
    }
    if (!cube_material.shader->Open("../basic_lighting/light_cube_v.glsl", "../basic_lighting/light_cube_f.glsl")) {
        assert(0);
    }
    if (!cube_material.shader->CompileAndLink()) {
        assert(0);
    }
}

static void load_image(GlContext *c, const char *path, struct image_data *out) {
	int width, height, nr_channels;

	stbi_set_flip_vertically_on_load(1);
	uint8_t *data = stbi_load(path, &out->width, &out->height, &out->nr_channel, 0);
	assert(data != NULL);
	out->data = data;
}

// load texutre returns texture id
static int load_texture(GlContext *c, const char *path) {
  struct image_data data;
  load_image(c, path, &data);

	unsigned int texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	// 为当前绑定的纹理对象设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int format = GL_RGB;
	if (data.nr_channel > 3)
		format = GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, format, data.width, data.height, 0, format, GL_UNSIGNED_BYTE, data.data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data.data);
	return texture_id;
}

static void init_texture(GlContext *c) {
  int id0 = load_texture(c, "../basic_lighting/container2.png");
  light_material.shader->SetUniformValues("texture0", id0);
  int id1 = load_texture(c, "../basic_lighting/container2_specular.png");
  light_material.shader->SetUniformValues("texture1", id1);

  light_material.texture_id[0] = id0;
  light_material.texture_id[1] = id1;
}

static void init_model(GlContext *c) {
    Model model;
    std::string path = "../basic_model/nanosuit/nanosuit.obj";
    model.Load(path, false);
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

  GLFWwindow *window = glfwCreateWindow(screen_width, screen_height, "hello", NULL, NULL);
  assert(window != NULL);
  glfwMakeContextCurrent(window);

  GlContext *context = new GlContext();

  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  glViewport(0, 0, screen_width, screen_height);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  init_scene();
  init_model(context);

  // load_shader(context);
  // init_vertices(context);
  // init_texture(context);

  glEnable(GL_DEPTH_TEST);

  while (!glfwWindowShouldClose(window)) {
    last_frame_time = current_frame_time;
    current_frame_time = glfwGetTime();

    process_input(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw(context);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  release_resource(context);
  delete context;
  glfwTerminate();
  return 0;
}
