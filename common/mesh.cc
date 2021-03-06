#include "mesh.h"

#include <stddef.h>

#include "camera.h"
#include "lo_common.h"
#include "scene.h"

static std::string GetUniformName(TextureType type, int idx) {
  if (type == kDiffuseTex) {
    return "diffuseTexture" + std::to_string(idx);
  } else if (type == kSpecularTex) {
    return "specularTexture" + std::to_string(idx);
  } else if (type == kNormalTex) {
    return "normalTexture" + std::to_string(idx);
  }
  return "";
}

Mesh::Mesh() {
  // create buffers/arrays
  glGenVertexArrays(1, &vao);
  CHECK_GL_ERROR;
  glGenBuffers(1, &vbo);
  CHECK_GL_ERROR;
  glGenBuffers(1, &ebo);
  CHECK_GL_ERROR;
}

Mesh::~Mesh() {
  glDeleteVertexArrays(1, &vao);
  CHECK_GL_ERROR;
  glDeleteBuffers(1, &vbo);
  CHECK_GL_ERROR;
  glDeleteBuffers(1, &ebo);
  CHECK_GL_ERROR;
  submit_done_ = false;
}

#if 0
static void InitShaderLight(GlContext *c, std::shared_ptr<Shader> shader) {
}

static void InitShaderMatrix(GlContext *c, std::shared_ptr<Shader> shader) {
  glm::mat4 model = glm::mat4(1.0);
  model = glm::translate(model, Vec3(1, 0, 0));
  // model = glm::scale(model, glm::vec3(1, 1, 1));
  // model = glm::rotate(model, glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f,
  // 0.5f)); model = glm::translate(model, cube_positions[i]);

  Mat4 view = GetWorld()->GetCamera()->GetViewMatrix();
  Mat4 projection = GetWorld()->GetCamera()->GetProjectionMatrix();
  Mat4 mvp = projection * view * model;
  glm::mat3 mv3x3 = glm::mat3x3(projection * view);

  unsigned int loc = shader->GetUniformLocation("model");
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model));

  loc = shader->GetUniformLocation("view");
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view));

  loc = shader->GetUniformLocation("projection");
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection));

  loc = shader->GetUniformLocation("mvp");
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mvp));

  loc = shader->GetUniformLocation("mv3x3");
  glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(mv3x3));
}

static void InitShader(GlContext *c, std::shared_ptr<Shader> shader) {
  InitShaderMatrix(c, shader);
  InitShaderLight(c, shader);
  shader->Use();
}
#endif

void Mesh::Render(MaterialPtr mat) {
#if 1
  int diffuse_idx = 0;
  int specular_idx = 0;
  int normal_idx = 0;
  std::string name;
  for (int i = 0; i < mat->textures.size(); i++) {
    auto tex = mat->textures[i];
    if (tex->type == kDiffuseTex) {
      name = "diffuseTexture" + std::to_string(diffuse_idx);
      diffuse_idx++;
    } else if (tex->type == kSpecularTex) {
      name = "specularTexture" + std::to_string(specular_idx);
      specular_idx++;
    } else if (tex->type == kNormalTex) {
      name = "normalTexture" + std::to_string(normal_idx);
      normal_idx++;
    } else {
      assert(0);
    }
    if (mat->shader->SetUniformValues(name.c_str(), i) >= 0) {
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_2D, tex->texture_id);
    } else {
      assert(0);
    }
  }
#endif

  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

  // glBindVertexArray(0);
  // glActiveTexture(GL_TEXTURE0);
}

// Submit data to GPU
void Mesh::Submit(MaterialPtr mat) {
  mat->UseShader();
  // if (submit_done_) return;
  glBindVertexArray(vao);
  CHECK_GL_ERROR;
  // load data into vertex buffers
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  CHECK_GL_ERROR;
  // A great thing about structs is that their memory layout is sequential for
  // all its items. The effect is that we can simply pass a pointer to the
  // struct and it translates perfectly to a glm::vec3/2 array which again
  // translates to 3/2 floats which translates to a byte array.
  if (need_submit_) {
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                 &vertices[0], GL_STATIC_DRAW);
    CHECK_GL_ERROR;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    CHECK_GL_ERROR;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);
    CHECK_GL_ERROR;
    need_submit_ = false;
  }

  // set the vertex attribute pointers
  // vertex Positions
  glEnableVertexAttribArray(0);
  CHECK_GL_ERROR;
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  CHECK_GL_ERROR;
  // vertex normals
  glEnableVertexAttribArray(1);
  CHECK_GL_ERROR;
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Normal));
  CHECK_GL_ERROR;
  // vertex texture coords
  glEnableVertexAttribArray(2);
  CHECK_GL_ERROR;
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, TexCoord));
  CHECK_GL_ERROR;
  // vertex tangent
  glEnableVertexAttribArray(3);
  CHECK_GL_ERROR;
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Tangent));
  CHECK_GL_ERROR;
  // vertex bitangent
  glEnableVertexAttribArray(4);
  CHECK_GL_ERROR;
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Bitangent));
  CHECK_GL_ERROR;

  // glBindVertexArray(0);
  submit_done_ = true;
}
