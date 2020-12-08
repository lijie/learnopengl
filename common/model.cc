#include "model.h"

bool Model::Load(const std::string &path, const std::string& texture_location,
    bool gamma, bool flip_uv) {
  // read file via ASSIMP
  Assimp::Importer importer;

  unsigned int flags = aiProcess_Triangulate | aiProcess_CalcTangentSpace;
  if (flip_uv)
      flags |= aiProcess_FlipUVs;
  const aiScene *scene =
      importer.ReadFile(path, flags);
  // check for errors
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    fprintf(stderr, "ERROR::ASSIMP:: %s \n", importer.GetErrorString());
    return false;
  }

  if (texture_location == "") {
      // retrieve the directory path of the filepath
      directory_ = path.substr(0, path.find_last_of('/'));
  }
  else {
      directory_ = texture_location;
      enable_texture_location_ = true;
  }

  // process ASSIMP's root node recursively
  ProcessNode(scene->mRootNode, scene);

  // check ?
  return true;
}

void Model::Render(MaterialPtr material) {
  for (auto i = 0; i < children_.size(); i++) {
    auto mat = material == nullptr ? children_[i]->material() : material;
    children_[i]->Submit(mat);
    children_[i]->Render(mat);
  }
}

void Model::ProcessNode(aiNode *node, const aiScene *scene) {
  // process each mesh located at the current node
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    // the node object only contains indices to index the actual objects in the
    // scene. the scene contains all the data, node is just to keep stuff
    // organized (like relations between nodes).
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    children_.push_back(ProcessMesh(mesh, scene));
  }
  // after we've processed all of the meshes (if any) we then recursively
  // process each of the children nodes
  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    ProcessNode(node->mChildren[i], scene);
  }
}

std::shared_ptr<Mesh> Model::ProcessMesh(aiMesh *mesh, const aiScene *scene) {
  // Mesh *_mesh = new Mesh();
  auto _mesh = make_shared<Mesh>();
  // data to fill
  // std::vector<Vertex> vertices;
  // std::vector<unsigned int> indices;
  // std::vector<Texture> textures;

  // walk through each of the mesh's vertices
  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;
    glm::vec3 vector;  // we declare a placeholder vector since assimp uses its
                       // own vector class that doesn't directly convert to
                       // glm's vec3 class so we transfer the data to this
                       // placeholder glm::vec3 first.
    // positions
    vector.x = mesh->mVertices[i].x;
    vector.y = mesh->mVertices[i].y;
    vector.z = mesh->mVertices[i].z;
    vertex.Position = vector;
    // normals
    vector.x = mesh->mNormals[i].x;
    vector.y = mesh->mNormals[i].y;
    vector.z = mesh->mNormals[i].z;
    vertex.Normal = vector;
    // texture coordinates
    if (mesh->mTextureCoords[0]) {
      Vec2 vec;
      // a vertex can contain up to 8 different texture coordinates. We thus
      // make the assumption that we won't use models where a vertex can have
      // multiple texture coordinates so we always take the first set (0).

      // TODO(andrewli):
      // 如何处理多套纹理坐标?
      vec.x = mesh->mTextureCoords[0][i].x;
      vec.y = mesh->mTextureCoords[0][i].y;
      vertex.TexCoord = vec;
    } else
      vertex.TexCoord = glm::vec2(0.0f, 0.0f);
    // tangent
    if (mesh->mTangents) {
        vector.x = mesh->mTangents[i].x;
        vector.y = mesh->mTangents[i].y;
        vector.z = mesh->mTangents[i].z;
        vertex.Tangent = vector;
    }
    // bitangent
    if (mesh->mBitangents) {
        vector.x = mesh->mBitangents[i].x;
        vector.y = mesh->mBitangents[i].y;
        vector.z = mesh->mBitangents[i].z;
        vertex.Bitangent = vector;
    }
    _mesh->vertices.push_back(vertex);
  }
  // now wak through each of the mesh's faces (a face is a mesh its triangle)
  // and retrieve the corresponding vertex indices.
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    // retrieve all indices of the face and store them in the indices vector
    for (unsigned int j = 0; j < face.mNumIndices; j++)
      _mesh->indices.push_back(face.mIndices[j]);
  }
  // process materials
  aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
  // we assume a convention for sampler names in the shaders. Each diffuse
  // texture should be named as 'texture_diffuseN' where N is a sequential
  // number ranging from 1 to MAX_SAMPLER_NUMBER. Same applies to other texture
  // as the following list summarizes: diffuse: texture_diffuseN specular:
  // texture_specularN normal: texture_normalN

#if 0
  material_t _material = std::make_shared<Material>();

  auto diffuse_texture =
      LoadMaterialTextures(material, aiTextureType_DIFFUSE, kDiffuseTex);
  if (diffuse_texture != nullptr) {
    _material->textures.push_back(diffuse_texture);
  }
  auto specular_texture =
      LoadMaterialTextures(material, aiTextureType_SPECULAR, kSpecularTex);
  if (specular_texture != nullptr) {
    _material->textures.push_back(specular_texture);
  }
  auto normal_texture =
      LoadMaterialTextures(material, aiTextureType_HEIGHT, kNormalTex);
  if (normal_texture != nullptr) {
    _material->textures.push_back(normal_texture);
  }

  // set default shader
  _material->shader = Shader::NewShader("../basic_model/model_shader");
  assert(_material->shader != nullptr);
#endif

  auto _material = NewSharedObject<PhongMaterial>();
  MaterialParams params;
  params.Albedo = Vec3(0.64, 0.32, 0.32);
  params.Specular = Vec3(0.2, 0.2, 0.2);
  params.Shininess = 1.0f;
  _material->SetParams(params);
  // return a mesh object created from the extracted mesh data
  // return new Mesh(vertices, indices, textures);
  _mesh->set_material(_material);

  return _mesh;
}

// 对于同一种类型的纹理, 目前只支持加载一张
//

texture_t Model::LoadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                      TextureType type_alias) {
  auto count = mat->GetTextureCount(type);
  if (count > 1) {
    fprintf(stderr, "ONLY SUPPORT ONE TEXTURE PER TYPE!\n");
  }
  if (count == 0) return nullptr;
  aiString str;
  mat->GetTexture(type, 0, &str);

  std::string file_name;
  if (enable_texture_location_) {
      std::string tex_path = std::string(str.C_Str());
      size_t pos = tex_path.find_last_of('/');
      if (pos == std::string::npos) {
          pos = tex_path.find_last_of('\\');
      }
      if (pos != std::string::npos) {
          file_name = tex_path.substr(pos + 1, std::string::npos);
      }
  }
  if (file_name.length() == 0) {
      file_name = std::string(str.C_Str());
  }

#ifdef __WIN32__
  std::string full_path = this->directory_ + "\\" + file_name;
#else
  std::string full_path = this->directory_ + "/" + file_name;
#endif

  fprintf(stdout, "Load Texutre: %s\n", full_path.c_str());
  return Texture::NewTexture(full_path, type_alias);
}

void Model::set_position(const Vec3& v) {
  Transform::set_position(v);
  auto children = GetChildren();
  for (size_t i = 0; i < children.size(); i++) {
    auto child = children[i];
    child->set_position(v);
  }
}
void Model::set_scale(const Vec3& v) {
  Transform::set_scale(v);
  auto children = GetChildren();
  for (size_t i = 0; i < children.size(); i++) {
    auto child = children[i];
    child->set_scale(v);
  }
}
