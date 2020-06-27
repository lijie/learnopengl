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
    vec3 lightDir = normalize(light_position - FragPos);
    vec3 diffuse = albedo * max(dot(Normal, lightDir), 0);

    vec3 viewDir = normalize(camera_position - FragPos);
    vec3 h = normalize(viewDir + lightDir);
    vec3 specular = albedo * pow(max(dot(Normal, h), 0), phong_exponent);

    FragColor = vec4(diffuse + specular, 1.0);
    // FragColor = vec4(albedo, 1.0);
}
