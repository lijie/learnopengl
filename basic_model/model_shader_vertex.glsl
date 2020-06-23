#version 330 core

// model space
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;
layout (location = 3) in vec3 a_tangent;
layout (location = 4) in vec3 a_bitangent;

// out vec3 FragPos;   // world space
out vec2 TexCoord;

// out vec3 CameraDirection_cs;   // camera spsace
// out vec3 LightDirection_cs;    // camera space
// out vec3 CameraDirection_ts;    // tangent space
// out vec3 LightDirection_ts;     // tangent space

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 mv3x3;
uniform mat4 mvp;

// uniform vec3 light_position;     // world space

void main()
{
    TexCoord = a_uv;
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
    // FragPos = (model * vec4(vertex_pos, 1.0)).xyz;
}
