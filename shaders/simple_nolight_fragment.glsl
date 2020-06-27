#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 light_position;
uniform vec3 light_color;
uniform float light_power;

uniform vec3 camera_position;
// uniform vec3 lightColor;
uniform vec3 albedo;

uniform float phong_exponent;

// uniform sampler2D texture0;
// uniform sampler2D texture1;

void main()
{
    FragColor = vec4(albedo, 1.0);
    // FragColor = vec4(albedo, 1.0);
}
