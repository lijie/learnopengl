#version 330 core
out vec4 FragColor;

// in vec3 FragPos;
// in vec3 Normal;
in vec2 TexCoord;

uniform vec3 lightPos;
uniform vec3 viewPos;
// uniform vec3 lightColor;
uniform vec3 albedo;

uniform sampler2D diffuseTexture0;

uniform sampler2D specularTexture0;

uniform sampler2D normalTexture0;

void main()
{
    // vec3 lightDir = normalize(lightPos - FragPos);
    // vec3 diffuse = albedo * max(dot(Normal, lightDir), 0) * texture(diffuseTexture0, TexCoord).rgb;
// 
    // vec3 viewDir = normalize(viewPos - FragPos);
    // vec3 h = normalize(viewDir + lightDir);
    // vec3 specular = albedo * pow(max(dot(Normal, h), 0), 32) * texture(specularTexture0, TexCoord).rgb;
// 
    // FragColor = vec4(diffuse + specular, 1.0);
    FragColor = texture(diffuseTexture0, TexCoord);
}
