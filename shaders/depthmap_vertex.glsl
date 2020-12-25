// model space
in vec3 aVertices;
out vec2 vHighPrecisionZW;

uniform mat4 uMVP;

// uniform vec3 light_position;     // world space
void main()
{
    gl_Position = uMVP * vec4(aVertices, 1.0);
    vHighPrecisionZW = gl_Position.zw;
}
