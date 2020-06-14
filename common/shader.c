#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "shader.h"

#define ERROR(fmt, ...) fprintf(stderr, "ERROR:SHADER[%s:%d]:" fmt, __func__, __LINE__, ##__VA_ARGS__)

struct shader {
	uint32_t vertex_shader;
	uint32_t fragment_shader;
	uint32_t program;
	const char *vertex_source;
	const char *fragment_source;
#define SHADER_FLAGS_EXTERNAL_SOURCE (1 << 0)
	uint32_t flags;
};

static const char * read_file(const char *path)
{
	FILE *f = fopen(path, "r");
	assert(f != NULL);

	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	rewind(f);

	char *ptr = (char *)malloc(size + 1);
	assert(ptr != NULL);

	fread(ptr, size, 1, f);
	ptr[size] = 0;
	return ptr;
}

static int read_shader_file(struct shader *shader, const char *vertex_path, const char *fragment_path)
{
	shader->vertex_source = read_file(vertex_path);
	shader->fragment_source = read_file(fragment_path);
	shader->flags &= ~(SHADER_FLAGS_EXTERNAL_SOURCE);
	return 0;
}

static struct shader * shader_create_object()
{
	struct shader *shader = (struct shader *)malloc(sizeof(struct shader));
	assert(shader != NULL);
	return shader;
}

static void shader_release_object(struct shader *shader)
{
	if (!(shader->flags & SHADER_FLAGS_EXTERNAL_SOURCE)) {
		free((void *)shader->vertex_source);
		free((void *)shader->fragment_source);
	}
	free(shader);
}

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

static int shader_compile_and_link(struct shader *shader)
{
	uint32_t vertex_shader;
	uint32_t fragment_shader;
	uint32_t shader_program;
	int res = 0;
	
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &shader->vertex_source, NULL);
	glCompileShader(vertex_shader);

	if (!check_shader_compile_success(vertex_shader)) {
		ERROR("vertex shader compile error.\n");
		res = -1;
		goto out;
	}

      	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &shader->fragment_source, NULL);
	glCompileShader(fragment_shader);

	if (!check_shader_compile_success(fragment_shader)) {
		ERROR("fragment shader compile error.\n");
		res = -1;
		goto out;
	}

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	if (!check_shader_link_success(shader_program)) {
		ERROR("shader link error.\n");
		res = -1;
		goto out;
	}

	shader->fragment_shader = fragment_shader;
	shader->vertex_shader = vertex_shader;
	shader->program = shader_program;

out:
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	return res;
}

struct shader * shader_create_with_source(const char *vertex_source, const char *fragment_source)
{
	struct shader *shader = shader_create_object();
	shader->vertex_source = vertex_source;
	shader->fragment_source = fragment_source;

	if (shader_compile_and_link(shader) != 0) {
		ERROR("shader compile error");
		shader_release_object(shader);
		return NULL;
	}

	return shader;
}

struct shader * shader_create(const char *vertex_path, const char *fragment_path)
{
	struct shader *shader = shader_create_object();

	if (read_shader_file(shader, vertex_path, fragment_path) != 0) {
		ERROR("read file %s, %s failed.\n", vertex_path, fragment_path);
		goto err_out;
	}

	if (shader_compile_and_link(shader) != 0) {
		ERROR("shader compile error.\n");
		goto err_out;
	}

	return shader;
err_out:
	shader_release_object(shader);
	return NULL;
}

void shader_use(struct shader *shader)
{
	glUseProgram(shader->program);
}

uint32_t shader_get_program(struct shader *shader)
{
	return shader->program;
}

int shader_get_uniform_location(struct shader *shader, const char *name)
{
	return glGetUniformLocation(shader->program, name);
}

void shader_close(struct shader *shader)
{
}

