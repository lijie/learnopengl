#ifndef __LEARNOPENGL_COMMON_INSTANCE_RENDERER_H__
#define __LEARNOPENGL_COMMON_INSTANCE_RENDERER_H__

#include <vector>
#include "lo_common.h"
#include "shape.h"

class InstanceRenderer : public Shape {
 public:
  InstanceRenderer(std::shared_ptr<Shape> r, unsigned int num);
  ~InstanceRenderer();
  void Submit(MaterialPtr mat) override;
  void Render(MaterialPtr mat) override;
  void Cleanup() override;

  void SetModel(unsigned int idx, const Mat4& model) {
    model_matrices_[idx] = model;
  }

  void Ready();

 private:
  std::shared_ptr<Shape> shape_;
  unsigned int num_;
  Mat4 *model_matrices_;
};

#endif  // __LEARNOPENGL_COMMON_INSTANCE_RENDERER_H__
