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

#include chunks/pack.glsl
#include chunks/common_uniforms.glsl
#include chunks/common.glsl
#include chunks/shadow_define.glsl

#if SPOT_SHADOW_NUM > 0
out vec4 vSpotShadowCoord[SPOT_SHADOW_NUM];
uniform mat4 spotShadowMatrix[SPOT_SHADOW_NUM];
#endif

void main()
{
    vec3 transformedNormal = uNormalMatrix * aNormal;
    vNormal = transformedNormal;

    vec3 worldPosition = vec3(uModel * vec4(aVertices, 1.0));

    vUV = aUV;

    vViewPosition = (uModelView * vec4(aVertices, 1.0)).xyz;
#if SPOT_SHADOW_NUM > 0
    vSpotShadowCoord[0] = spotShadowMatrix[0] * vec4(worldPosition, 1.0);
#endif

    gl_Position = uMVP * vec4(aVertices, 1.0);
}
