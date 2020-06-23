#version 330 core

layout (location = 0) in vec3 vertex_pos;

uniform mat4 mvp;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 mv3x3;

void main()
{
    gl_Position = mvp * vec4(vertex_pos, 1.0);
}
