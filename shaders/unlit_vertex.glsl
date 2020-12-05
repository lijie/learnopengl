// model space
in vec3 aVertices;
in vec2 aUV;

out vec2 vUV;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uMV3x3;
uniform mat4 uMVP;
uniform mat4 uModelView;

void main()
{
    vUV = aUV;
    gl_Position = uMVP * vec4(aVertices, 1.0);
}
