#version 330 core
layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_uv;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 mvp;
uniform mat3 normal_model;

void main()
{
    // FragPos 直接计算结果是 aPos 的世界坐标
    // 但是作为 out 数据传递给 fragment shader 时, opengl 会在三角形上差值从而获得当前fragment的世界坐标
    FragPos = vec3(model * vec4(vertex_pos, 1.0));
    // 移到cpu计算
    // Normal = mat3(transpose(inverse(model))) * aNormal;
    Normal = normal_model * vertex_normal;
    TexCoord = vertex_uv;

    gl_Position = mvp * vec4(vertex_pos, 1.0);
}
