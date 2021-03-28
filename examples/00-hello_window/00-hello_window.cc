#include <stdint.h>
#include <stdio.h>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <assert.h>
#include "shader.h"

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

#if 1
static int check_shader_compile_success(uint32_t shader)
{
	int success = 0;
	char info_log[512];

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, info_log);
		fprintf(stderr, "ERROR: SHADER: %s\n", info_log);
	}

	return success;
}

static int check_shader_link_success(uint32_t program)
{
	int success = 0;
	char info_log[512];

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, info_log);
		fprintf(stderr, "ERROR: PROGRAM: %s\n", info_log);
	}

	return success;
}
#endif

static void draw_triangle(uint32_t vao)
{
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

static float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.0f, 0.5f, 0.0f,
};

static void prepare_triangle_data(uint32_t *vao_out)
{
	uint32_t vbo;
	uint32_t vao;

	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// paramter1 0: see vertex shader source location = 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

        auto *shader = new Shader();
		shader->Open("00-hello_window/00-vs.glsl", "00-hello_window/00-fs.glsl");
        shader->CompileAndLink();
	shader->Use();
	*vao_out = vao;
}

static float rectangle_vertices[] = {
	0.5f, 0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	-0.5f, -0.5f, 0.0f,
	-0.5f, 0.5f, 0.0f,
};

static uint32_t rectange_indices[] = {
	0, 1, 3,
	1, 2, 3,
};

static void draw_rectangle(uint32_t vao)
{
	// glUseProgram(program);
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

static void prepare_rectangle_data(uint32_t *vao_out)
{
	uint32_t vbo;
	uint32_t vao;
	uint32_t ebo;

	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vertices), rectangle_vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectange_indices), rectange_indices, GL_STATIC_DRAW);

	// paramter1 0: see vertex shader source location = 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// struct shader *shader = shader_create_with_source(vertex_shader_source, fragment_shader_source);
        auto shader = new Shader();
		shader->Open("00-hello_window/00-vs.glsl", "00-hello_window/00-fs.glsl");
        shader->CompileAndLink();
        shader->Use();
	// shader_use(shader);
	*vao_out = vao;
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

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	uint32_t vao;
	// prepare_triangle_data(&vao);
	prepare_rectangle_data(&vao);

	while (!glfwWindowShouldClose(window)) {
		process_input(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// draw_triangle(vao);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		draw_rectangle(vao);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	
	return 0;
}
