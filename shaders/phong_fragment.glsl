
#define PI 3.141592653589793
#define RECIPROCAL_PI 0.3183098861837907
#define RreflectDirectFunc      RreflectDirectBlinnPhong

#ifdef USE_DIFFUSE_TEXTURE
uniform sampler2D uDiffuseTexture;
#endif
#ifdef USE_SPECULAR_TEXTURE
uniform sampler2D uSpecularTexture;
#endif

uniform vec3 uAlbedo;
uniform vec3 uSpecular;
uniform vec3 uAmbientLight;
uniform float uShininess;

in vec2 vUV;
in vec3 vNormal;
in vec3 vViewPosition;
out vec4 FragColor;

vec3 testColor;

#include chunks/light_define.glsl

void main()
{
#ifdef USE_DIFFUSE_TEXTURE
    vec4 diffuseColor = texture(uDiffuseTexture, vUV);
#else
    vec4 diffuseColor = vec4(uAlbedo, 1.0);
#endif

#ifdef USE_SPECULAR_TEXTURE
    float specularStrength = texture2D(uSpecularTexture, vUV).r;
#else
    float specularStrength = 1.0;
#endif

#ifdef USE_NORMAL_TEXTURE
    // [0, 1] -> [-1, 1]
    vec3 normal = texture2D(uNormalTexture, vUV) * 2.0 - 1.0;
    normal = normalize(uNormalMatrix, normal);
#else
    vec3 normal = vNormal;
#endif

    vec3 view_direction = normalize(-vViewPosition);

    BlinnPhongMaterial material;
    material.diffuseColor = diffuseColor.rgb;
    material.specularColor = uSpecular;
    material.specularShininess = uShininess;
    material.specularStrength = specularStrength;
    GeometricContext geometry;
    geometry.position = vViewPosition;
    geometry.normal = normal;
    geometry.viewDir = normalize(view_direction);

    IncidentLight directLight;
    ReflectedLight reflectedLight;

#if DIRECTION_LIGHT_NUM > 0
    DirectionalLight directionalLight;

    directionalLight = directionalLights[ 0 ];
    getDirectionalDirectLightIrradiance(directionalLight, geometry, directLight);
    RreflectDirectBlinnPhong(directLight, geometry, material, reflectedLight);
#endif

#if POINT_LIGHT_NUM > 0
    PointLight pointLight;
    pointLight = pointLights[0];
    getPointDirectLightIrradiance(pointLight, geometry, directLight);
    RreflectDirectBlinnPhong(directLight, geometry, material, reflectedLight);
#endif

    vec3 irradiance = getAmbientLightIrradiance(uAmbientLight);
    ReflectIndirectBlinnPhong(irradiance, geometry, material, reflectedLight);

    vec3 outLight = reflectedLight.directDiffuse + reflectedLight.directSpecular + reflectedLight.indirectDiffuse;

    FragColor = vec4(outLight, diffuseColor.a);
    // FragColor = vec4(reflectedLight.directSpecular, diffuseColor.a);
    // testColor = normal;
    // FragColor = vec4(testColor, diffuseColor.a);
    FragColor = LinearTosRGB(FragColor);
}
