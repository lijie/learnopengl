#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 camera_position;
uniform vec3 albedo;

uniform sampler2D texture0;
uniform float transparent_threshold;

void main()
{
    vec4 tex_color = texture(texture0, TexCoord);
    if (tex_color.a < transparent_threshold)
        discard;
    FragColor = vec4(albedo, 1.0) * tex_color;
}
