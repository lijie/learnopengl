#version 330 core

// model space
layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_uv;
layout (location = 3) in vec3 vertex_tangent;
layout (location = 4) in vec3 vertex_bitangent;

out vec3 pos_ws;   // world space
out vec2 TexCoord;

// out vec3 CameraDirection_cs;   // camera spsace
// out vec3 LightDirection_cs;    // camera space
out vec3 CameraDirection_ts;    // tangent space
out vec3 LightDirection_ts;     // tangent space

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 mv3x3;
uniform mat4 mvp;

uniform vec3 light_position;     // world space

void main()
{
    TexCoord = vertex_uv;
    pos_ws = (model * vec4(vertex_pos, 1.0)).xyz;
    gl_Position = mvp * vec4(vertex_pos, 1.0);

    // position at camera space
    vec3 pos_cs = (view * model * vec4(vertex_pos, 1.0)).xyz;
    // 当前位置 -> 摄像机
    vec3 CameraDirection_cs = vec3(0, 0, 0) - pos_cs;

    // 光源在摄像机空间的位置
    vec3 light_position_cs = (view * model * vec4(light_position, 1.0)).xyz;
    // 当前位置 -> 光源
    vec3 LightDirection_cs = light_position_cs + CameraDirection_cs;

    // to cameral space
    vec3 vertex_tangent_cs = mv3x3 * vertex_tangent;
    vec3 vertex_bitangent_cs = mv3x3 * vertex_bitangent;
    vec3 vertex_normal_cs = mv3x3 * vertex_normal;

    mat3 tbn = transpose(mat3(
        vertex_tangent_cs,
        vertex_bitangent_cs,
        vertex_normal_cs
    ));

    // to tangent space
    CameraDirection_ts = tbn * CameraDirection_cs;
    LightDirection_ts = tbn * LightDirection_cs;
}
