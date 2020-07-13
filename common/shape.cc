#include "shape.h"

#include "glad/glad.h"
#include "lo_common.h"
#include "material.h"
#include "shader.h"
#include "camera.h"
#include "scene.h"

Shape::Shape() {
  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glGenBuffers(1, &ebo_);
}

Shape::~Shape() {
  glDeleteVertexArrays(1, &vao_);
  glDeleteBuffers(1, &vbo_);
  glDeleteBuffers(1, &ebo_);
}

void Shape::Translate(const Vec3& v) {
  set_position(v);
  // model_ = glm::translate(model_, v);
}

void Shape::Scale(const Vec3& v) {
  set_scale(v);
  // model_ = glm::scale(model_, v);
}

void Shape::Render() {
  glBindVertexArray(vao_);
  if (enable_instance_) {
    glDrawArraysInstanced(GL_TRIANGLES, 0, vertex_size_, instance_num_);
    // glDrawArrays(GL_TRIANGLES, 0, vertex_size_);
  } else {
    glDrawArrays(GL_TRIANGLES, 0, vertex_size_);
  }
}

void Shape::Submit() {
  material_->UseShader(this);

  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);

  auto size = vertex_size_ * 8 * sizeof(float);
  glBufferData(GL_ARRAY_BUFFER, size, vertices_, GL_STATIC_DRAW);

  for (size_t i = 0; i < vao_attr_vec_.size(); i++) {
    const VAOAttr& attr = vao_attr_vec_[i];

    if (attr.external_vbo >= 0) {
      glBindBuffer(GL_ARRAY_BUFFER, attr.external_vbo);
    }
    glVertexAttribPointer(i, attr.size, attr.type, attr.normalize, attr.stride, (void *)attr.offset);
    glEnableVertexAttribArray(i);
    if (attr.divisor > 0) {
      glVertexAttribDivisor(i, attr.divisor);
    }
  }

  glBindVertexArray(0);

  if (before_render_func_ != nullptr)
    before_render_func_();
}

void Shape::Cleanup() {
  if (after_render_func_ != nullptr)
    after_render_func_();
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

Cube::Cube() : Shape() {
  set_vertices(&cube_vertices[0], 36);

  // set vao attr
  VAOAttr attr;
  // pos attr
  attr.Reset(3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
  vao_attr_vec_.push_back(attr);
  // normal attr
  attr.Reset(3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 3 * sizeof(float));
  vao_attr_vec_.push_back(attr);
  // texcoord attr
  attr.Reset(2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 6 * sizeof(float));
  vao_attr_vec_.push_back(attr);
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

    // set vao attr
  VAOAttr attr;
  // pos attr
  attr.Reset(3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
  vao_attr_vec_.push_back(attr);
  // normal attr
  attr.Reset(3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 3 * sizeof(float));
  vao_attr_vec_.push_back(attr);
  // texcoord attr
  attr.Reset(2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 6 * sizeof(float));
  vao_attr_vec_.push_back(attr);
}

static float quad_vertices[] = {
  // positions        // normal  // texture Coords
  0.5f, -0.5f, 0.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // A
  0.5f, 0.5f,  0.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // B
  -0.5f, 0.5f,  0.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // C

  -0.5f, 0.5f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f, // C
  -0.5f, -0.5f, 0.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f, // D
  0.5f, -0.5f, 0.0f,  0.0f,  0.0f, 1.0f, 1.0f, 0.0f,  // A
};

Quad::Quad() {
  set_vertices(quad_vertices, 6);
  // set vao attr
  VAOAttr attr;
  // pos attr
  attr.Reset(3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
  vao_attr_vec_.push_back(attr);
  // normal attr
  attr.Reset(3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 3 * sizeof(float));
  vao_attr_vec_.push_back(attr);
  // texcoord attr
  attr.Reset(2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 6 * sizeof(float));
  vao_attr_vec_.push_back(attr);
}

LightSource::LightSource() {
  material_ = std::make_shared<Material>();
  material_->shader = Shader::NewShader("../shaders/light_source");
  set_color(Vec3(1.0, 1.0, 1.0));
}
