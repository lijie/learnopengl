#include "instance_renderer.h"

#include "glad/glad.h"

InstanceRenderer::InstanceRenderer(std::shared_ptr<Shape> r, unsigned int num) {
  shape_ = r;
  num_ = num;
  model_matrices_ = new glm::mat4[num];
}

InstanceRenderer::~InstanceRenderer() {
  delete[] model_matrices_;
}

void InstanceRenderer::Ready() {
  // prepare vbo
  unsigned int vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, num_ * sizeof(Mat4), &model_matrices_[0],
               GL_STATIC_DRAW);

  for (int i = 0; i < 4; i++) {
    VAOAttr attr;
    attr.Reset(4, GL_FLOAT, GL_FALSE, sizeof(Mat4), i * sizeof(Vec4));
    if (i == 0) {
      attr.external_vbo = vbo;
    }
    attr.divisor = 1;
    vao_attr_vec_.push_back(attr);
    shape_->append_vao_attr(attr);
  }

  shape_->set_material(material());
  shape_->enable_instance(true, num_);
}

void InstanceRenderer::Render() { shape_->Render(); }
void InstanceRenderer::Submit() { shape_->Submit(); }
void InstanceRenderer::Cleanup() { shape_->Cleanup(); }
