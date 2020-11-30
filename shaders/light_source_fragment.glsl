
out vec4 FragColor;

in vec2 TexCoord;

uniform vec3 albedo;
uniform sampler2D texture0;

void main()
{
    FragColor = vec4(albedo * texture(texture0, TexCoord).rgb, 1.0);
}
