// model space
in vec3 aVertices;
in vec3 aNormal;
in vec2 aUV;

#ifdef ATTR_HAS_TANGENT
in vec3 aTangent;
#endif
#ifdef ATTR_HAS_BITANGENT
in vec3 aBitangent;
#endif

out vec2 vUV;
out vec3 vNormal;
out vec3 vViewPosition;      // position in view space

#include chunks/common_uniforms.glsl

void main()
{
    vec3 transformedNormal = uNormalMatrix * aNormal;
    vNormal = transformedNormal;

    vUV = aUV;

    vViewPosition = (uModelView * vec4(aVertices, 1.0)).xyz;

    gl_Position = uMVP * vec4(aVertices, 1.0);
}
