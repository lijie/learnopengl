#version 330 core
out vec4 FragColor;

in vec3 pos_ws;   // world space
in vec2 TexCoord;
// in vec3 CameraDirection_cs;   // camera spsace
// in vec3 LightDirection_cs;    // camera space
in vec3 CameraDirection_ts;    // tangent space
in vec3 LightDirection_ts;     // tangent space

uniform vec3 light_color;
uniform float light_power;
uniform vec3 light_position;     // world space
uniform float phong_exponent;

uniform sampler2D diffuseTexture0;
//uniform sampler2D diffuseTexture1;
uniform sampler2D specularTexture0;
//uniform sampler2D specularTexture1;
uniform sampler2D normalTexture0;
//uniform sampler2D normalTexture1;

void main()
{

    vec3 diffuse_color = texture(diffuseTexture0, TexCoord).rgb;
    vec3 ambient_color = vec3(0.1, 0.1, 0.1) * diffuse_color;
    vec3 specular_color = texture(specularTexture0, TexCoord).rgb;

    // get normal from normal texture in tangent space
    vec3 normal_ts = normalize(texture(normalTexture0, vec2(TexCoord.x, TexCoord.y)).rgb * 2.0 - 1.0);

    // distance between fragment and light
    float light_distance = length(light_position - pos_ws);

    vec3 n = normal_ts;
    vec3 l = normalize(LightDirection_ts);
    float cos_theta = clamp(dot(n, l), 0, 1);                   

    vec3 e = normalize(CameraDirection_ts);
    vec3 r = reflect(-l, n);
    float cos_alpha = clamp(dot(e, r), 0, 1);

    // FragColor = vec4(ambient_color +
    //     diffuse_color * light_color * light_power * cos_theta / (light_distance * light_distance) +
    //     specular_color * light_color * light_power * pow(cos_alpha, phong_exponent) /
    //     (light_distance * light_distance), 1.0);

    FragColor = vec4(diffuse_color, 1.0);
}
