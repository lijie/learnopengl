#version 330 core

layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_uv;

uniform mat4 mvp;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 mv3x3;

void main()
{
    gl_Position = mvp * vec4(vertex_pos, 1.0);
}
