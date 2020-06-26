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
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
}

Mesh::~Mesh() {
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
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

void Mesh::Render(GlContext *ctx) {
  // InitShader(ctx, material->shader);
  material->UseShader(this);

  Submit();

#if 1
  int diffuse_idx = 0;
  int specular_idx = 0;
  int normal_idx = 0;
  std::string name;
  for (int i = 0; i < material->textures.size(); i++) {
    auto tex = material->textures[i];
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
    if (material->shader->SetUniformValues(name.c_str(), i) >= 0) {
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
void Mesh::Submit() {
  // if (submit_done_) return;
  glBindVertexArray(vao);
  // load data into vertex buffers
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // A great thing about structs is that their memory layout is sequential for
  // all its items. The effect is that we can simply pass a pointer to the
  // struct and it translates perfectly to a glm::vec3/2 array which again
  // translates to 3/2 floats which translates to a byte array.
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0],
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               &indices[0], GL_STATIC_DRAW);

  // set the vertex attribute pointers
  // vertex Positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  // vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Normal));
  // vertex texture coords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, TexCoord));
  // vertex tangent
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Tangent));
  // vertex bitangent
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Bitangent));

  // glBindVertexArray(0);
  submit_done_ = true;
}
