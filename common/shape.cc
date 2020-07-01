#include "shape.h"

#include "glad/glad.h"
#include "lo_common.h"
#include "material.h"
#include "shader.h"
#include "camera.h"
#include "scene.h"

Shape::Shape() {
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
}

Shape::~Shape() {
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
}

void Shape::Translate(const Vec3& v) {
  set_position(v);
  // model_ = glm::translate(model_, v);
}

void Shape::Scale(const Vec3& v) {
  set_scale(v);
  // model_ = glm::scale(model_, v);
}

void Shape::Render(GlContext* ctx) {
  // glm::mat4 model = glm::mat4(1.0);
  // model = glm::translate(model, position_);
  // model = glm::scale(model, scale_);

  if (before_render_func_ != nullptr)
    before_render_func_();
  material_->UseShader(this);
  Submit();
  glBindVertexArray(vao);

  // glStencilMask(stencil_mask_);
  glDrawArrays(GL_TRIANGLES, 0, vertex_size_);
  if (after_render_func_ != nullptr)
    after_render_func_();
}

void Shape::Submit() {
// if (submit_done_) return;
  glBindVertexArray(vao);
  // load data into vertex buffers
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // A great thing about structs is that their memory layout is sequential for
  // all its items. The effect is that we can simply pass a pointer to the
  // struct and it translates perfectly to a glm::vec3/2 array which again
  // translates to 3/2 floats which translates to a byte array.
  auto size = vertex_size_ * 8 * sizeof(float);
  glBufferData(GL_ARRAY_BUFFER, size, vertices_, GL_STATIC_DRAW);

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

  glBindVertexArray(0);
}

// cube vertices and normal
static float cube_vertices[] = {
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

Cube::Cube(): Shape() {
    set_vertices(&cube_vertices[0], 36);
}

void Cube::Submit() {
  // if (submit_done_) return;
  glBindVertexArray(vao);
  // load data into vertex buffers
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // A great thing about structs is that their memory layout is sequential for
  // all its items. The effect is that we can simply pass a pointer to the
  // struct and it translates perfectly to a glm::vec3/2 array which again
  // translates to 3/2 floats which translates to a byte array.
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), &cube_vertices[0],
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

  glBindVertexArray(0);
}

float plane_vertices[] = {
  // positions        // normal  // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
  5.0f, 0.0f,  5.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
  -5.0f, 0.0f,  5.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
  -5.0f, 0.0f, -5.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
  5.0f, 0.0f,  5.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
  -5.0f, 0.0f, -5.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
  5.0f, 0.0f, -5.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f
};

// float plane_vertices[] = {
// };

Plane::Plane(): Shape() {
    set_vertices(&plane_vertices[0], 6);
}

void Plane::Submit() {
  // if (submit_done_) return;
  glBindVertexArray(vao);
  // load data into vertex buffers
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // A great thing about structs is that their memory layout is sequential for
  // all its items. The effect is that we can simply pass a pointer to the
  // struct and it translates perfectly to a glm::vec3/2 array which again
  // translates to 3/2 floats which translates to a byte array.
  glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), &plane_vertices[0],
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

  glBindVertexArray(0);
}

static float quad_vertices[] = {
  // positions        // normal  // texture Coords
  0.5f, -0.5f, 0.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
  0.5f, 0.5f,  0.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
  -0.5f, 0.5f,  0.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,

  0.5f, -0.5f, 0.0f,  0.0f,  0.0f, 1.0f, 1.0f, 0.0f,
  -0.5f, 0.5f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f,
  -0.5f, -0.5f, 0.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,
};

Quad::Quad() {
  set_vertices(quad_vertices, 6);
}

LightSource::LightSource() {
  material_ = std::make_shared<Material>();
  material_->shader = Shader::NewShader("../shaders/light_source");
  set_color(Vec3(1.0, 1.0, 1.0));
}
