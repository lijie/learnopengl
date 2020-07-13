#version 330 core
layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec2 vertex_normal;
layout (location = 2) in vec2 vertex_uv;
layout (location = 3) in mat4 vertex_instance_model;

out vec2 TexCoord;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoord = vertex_uv;
    gl_Position = projection * view * vertex_instance_model * vec4(vertex_pos, 1.0);
}
