#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 viewPos;
// uniform vec3 lightColor;
uniform vec3 albedo;

void main()
{
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 diffuse = albedo * max(dot(Normal, lightDir), 0);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 h = normalize(viewDir + lightDir);
    vec3 specular = albedo * pow(max(dot(Normal, h), 0), 32);

    FragColor = vec4(diffuse + specular, 1.0);
}
