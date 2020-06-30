#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 camera_position;
uniform vec3 albedo;

uniform sampler2D texture0;

void main()
{
    FragColor = vec4(albedo * texture(texture0, TexCoord).rgb, 1.0);
}
