#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // FragPos 直接计算结果是 aPos 的世界坐标
    // 但是作为 out 数据传递给 fragment shader 时, opengl 会在三角形上差值从而获得当前fragment的世界坐标
    FragPos = vec3(model * vec4(aPos, 1.0));
    // 移到cpu计算
    Normal = mat3(transpose(inverse(model))) * aNormal;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
