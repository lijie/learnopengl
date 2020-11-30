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

// out vec3 vPositionWS;   // world space
out vec2 vUV;
out vec3 vNormal;
out vec3 vViewPosition;      // position in view space

// out vec3 CameraDirection_cs;   // camera spsace
// out vec3 LightDirection_cs;    // camera space
// out vec3 vCameraDirectionTS;    // tangent space
// out vec3 vLightDirectionTS;     // tangent space

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uMV3x3;
uniform mat4 uMVP;
uniform mat4 uModelView;
uniform mat3 uNormalMatrix;

#ifdef FLAT_SHADED
#endif

// uniform vec3 light_position;     // world space
void main()
{
    vec3 transformedNormal = uNormalMatrix * aNormal;
    vNormal = transformedNormal;

    vUV = aUV;

    vViewPosition = (uModelView * vec4(aVertices, 1.0)).xyz;

    gl_Position = uMVP * vec4(aVertices, 1.0);
}
