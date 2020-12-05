#include "cubemap.h"

#include <assert.h>
#include "stb_image.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "lo_common.h"
#include "material.h"
#include "scene.h"
#include "camera.h"

static float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

CubeMap::CubeMap(const std::vector<std::string> &path) {
    // init vertices
    set_vertices(&skyboxVertices[0], 36);

    // set vao attr
    VAOAttr attr;
    // pos attr
    attr.Reset(3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    vao_attr_vec_.push_back(attr);

    // init textures
    unsigned int tex_id;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex_id);

    stbi_set_flip_vertically_on_load(0);
    int width, height, channel;
    for (size_t i = 0; i < path.size(); i++) {
        uint8_t *data = stbi_load(path[i].c_str(), &width, &height, &channel, 0);
        assert(data != NULL);
        int format = GL_RGB;
        if (channel == 4)
            format = GL_RGBA;
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    texture_id_ = tex_id;

    auto tex = Texture::NewTextureWithTextureId(tex_id);
    tex->is_cube_map = true;
    // init material
    auto mat = make_shared<Material>("../shaders/skybox");
    mat->SetProperty("skybox", tex);
    set_material(mat);
}

void CubeMap::Submit(MaterialPtr mat) {
    glm::mat4 view = glm::mat4(glm::mat3(GetWorld()->GetCamera()->GetViewMatrix()));
    material_->SetProperty("view", view);

    Shape::Submit(mat);
}
