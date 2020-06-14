#include <stdint.h>
#include <stdio.h>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <assert.h>
#include <math.h>
#include "shader.h"
#include "context.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static const char * vertex_shader_path = "../texture/vertex.glsl";
static const char * fragment_shader_path = "../texture/fragment.glsl";

static int screen_width = 800;
static int screen_height = 600;

struct image_data {
	uint8_t *data;
	int width;
	int height;
	int nr_channel;
};

static void normalize_size(int width, int height, float* points)
{
	while (width > screen_width) {
		width = width / 2;
		height = height / 2;
	}
	while (height > screen_height) {
		width = width / 2;
		height = height / 2;
	}

	float scale_w = (float)width / (float)screen_width;
	float scale_h = (float)height / (float)screen_height;

	points[0] = scale_w / 2;
	points[1] = scale_h / 2;
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

static void process_input(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, 1);
	}
}

static void draw(GlContext *c)
{
	glBindTexture(GL_TEXTURE_2D, c->texture_ids[0]);
        c->shader->Use();
	glBindVertexArray(c->vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

float vertices[] = {
//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
	-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,    // 左上
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
};

static void reset_vertices_with_image(struct image_data *image, float *vertices)
{
#define __OFFSET	8
	float points[2];
	normalize_size(image->width, image->height, points);

	float x = points[0];
	float y = points[1];

	// printf("%f, %f\n", x, y);

	vertices[0] = -x;
	vertices[1] = y;

	vertices[0 + __OFFSET] = x;
	vertices[1 + __OFFSET] = y;

	vertices[0 + __OFFSET * 2] = -x;
	vertices[1 + __OFFSET * 2] = -y;

	vertices[0 + __OFFSET * 3] = x;
	vertices[1 + __OFFSET * 3] = -y;
#undef __OFFSET
}

static void init_vertices(GlContext *c, struct image_data *image)
{
	static unsigned int indices[] = {
//		0, 1, 3,
//		1, 2, 3,
		0, 1, 3,
		0, 2, 3,
	};

	reset_vertices_with_image(image, vertices);

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	c->vao = VAO;
	c->vbo = VBO;
	c->ebo = EBO;
}

static void load_shader(GlContext *c)
{
  auto *shader = new Shader();
  if (!shader->Open(vertex_shader_path, fragment_shader_path)) {
    assert(0);
  }
  if (!shader->CompileAndLink()) {
    assert(0);
  }
  shader->Use();
  c->shader = shader;
}

static void load_image(GlContext *c, struct image_data *out)
{
	int width, height, nr_channels;

	stbi_set_flip_vertically_on_load(1);
	uint8_t *data = stbi_load("../texture/zelda.png", &out->width, &out->height, &out->nr_channel, 0);
	assert(data != NULL);
	out->data = data;
}

static void load_texture(GlContext *c, struct image_data *in)
{
	unsigned int texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	// 为当前绑定的纹理对象设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int format = GL_RGB;
	if (in->nr_channel > 3)
		format = GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, format, in->width, in->height, 0, format, GL_UNSIGNED_BYTE, in->data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(in->data);
	c->texture_ids[0] = texture_id;

	// glUniform1i(shader_get_uniform_location(c->shader, "ourTexture"), 0);
        c->shader->SetUniformValues("ourTexture", 0);
}

static void release_resource(GlContext *c)
{
	glDeleteVertexArrays(1, &c->vao);
	glDeleteBuffers(1, &c->vbo);
	glDeleteBuffers(1, &c->ebo);
        delete c->shader;
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(800, 600, "hello", NULL, NULL);
	assert(window != NULL);
	glfwMakeContextCurrent(window);

	GlContext *context = new GlContext();

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	struct image_data image;
	load_image(context, &image);
	load_shader(context);
	init_vertices(context, &image);
	load_texture(context, &image);

	while (!glfwWindowShouldClose(window)) {
		process_input(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		draw(context);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	release_resource(context);
        delete context;
	glfwTerminate();
	return 0;
}
