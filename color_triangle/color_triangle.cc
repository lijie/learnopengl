#include <stdint.h>
#include <stdio.h>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <assert.h>
#include <math.h>
#include "shader.h"
#include "context.h"

static const char * vertex_shader_path = "../color_triangle/vertex2.glsl";
static const char * fragment_shader_path = "../color_triangle/fragment2.glsl";

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

static void draw_triangle(GlContext *c)
{
	double time_value = glfwGetTime();
	double green_value = (sin(time_value) / 2.0f) + 0.5f;
        // c->shader->SetUniformValues("ourColor", 0.0, green_value, 0.0, 1.0);
        c->shader->Use();
	// shader_uniform_set_float4(c->shader, "ourColor", 0.0f, green_value, 0.0f, 1.0f);
	// shader_use(c->shader);

	glBindVertexArray(c->vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

// static float vertices[] = {
// 	-0.5f, -0.5f, 0.0f,
// 	0.5f, -0.5f, 0.0f,
// 	0.0f, 0.5f, 0.0f,
// };

static float vertices[] = {
    // 位置              // 颜色
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
};

static void prepare_triangle_data(GlContext *c)
{
	uint32_t vbo;
	uint32_t vao;

	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// paramter1 0: see vertex shader source location = 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// struct shader *shader = shader_create_with_source(vertex_shader_source, fragment_shader_source);
	// struct shader *shader = shader_create(vertex_shader_path, fragment_shader_path);
        auto *shader = new Shader();
        shader->Open(vertex_shader_path, fragment_shader_path);
        shader->CompileAndLink();
        shader->Use();
	// shader_use(shader);

	// output
	c->shader = shader;
	c->vao = vao;
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

	prepare_triangle_data(context);

	while (!glfwWindowShouldClose(window)) {
		process_input(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		draw_triangle(context);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

        delete context;
	glfwTerminate();
	return 0;
}
