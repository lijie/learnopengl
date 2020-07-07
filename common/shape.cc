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

void Shape::Render() {
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, vertex_size_);
}

void Shape::Submit() {
  material_->UseShader(this);

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

  for (size_t i = 0; i < vao_attr_vec_.size(); i++) {
    const VAOAttr& attr = vao_attr_vec_[i];
    glVertexAttribPointer(i, attr.size, attr.type, attr.normalize, attr.stride, (void *)attr.offset);
    glEnableVertexAttribArray(i);
  }

#if 0
   // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // normal
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // texture coord
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
#endif

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
