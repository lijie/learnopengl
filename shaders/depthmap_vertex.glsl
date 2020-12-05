// model space
in vec3 aVertices;

// uniform mat4 uModel;
// uniform mat4 uView;
// uniform mat4 uProjection;
// uniform mat3 uMV3x3;
uniform mat4 uMVP;
// uniform mat4 uModelView;
// uniform mat3 uNormalMatrix;

// uniform vec3 light_position;     // world space
void main()
{
    gl_Position = uMVP * vec4(aVertices, 1.0);
}
