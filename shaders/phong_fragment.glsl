
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

#include chunks/pack.glsl
#include chunks/light_define.glsl
#include chunks/shadow_define.glsl

#if SPOT_SHADOW_NUM > 0
in vec4 vSpotShadowCoord[SPOT_SHADOW_NUM];
#endif

float ShadowCalculation(vec4 fragPosLightSpace)
{
    float shoadowBias = 0.001;
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(spotShadowMap[0], projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > (closestDepth + shoadowBias)  ? 1.0 : 0.0;

    return shadow;
}


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

#if SPOT_LIGHT_NUM > 0
    SpotLight spotLight;
    spotLight = spotLights[0];
    // void getSpotDirectLightIrradiance(const in SpotLight spotLight, const in GeometricContext geometry, out IncidentLight directLight) {
    getSpotDirectLightIrradiance(spotLight, geometry, directLight);
#if SPOT_SHADOW_NUM > 0
    // SpotLightShadow spotLightShadow = spotLightShadows[0];
    // float shadow = all(bvec2(directLight.visible, true)) ? getShadow(spotShadowMap[0], spotLightShadow.shadowMapSize, spotLightShadow.shadowBias, spotLightShadow.shadowRadius, vSpotShadowCoord[0]) : 1.0;
    // directLight.color *= shadow;
    // // float z1 = vSpotShadowCoord[0].z / vSpotShadowCoord[0].w;
    // float z1 = pow(shadow, 256);
    // testColor = vec3(shadow);
    float shadow = ShadowCalculation(vSpotShadowCoord[0]);
    testColor = vec3(shadow);
    directLight.color *= (1 - shadow);
#endif
#endif
    RreflectDirectBlinnPhong(directLight, geometry, material, reflectedLight);

    vec3 irradiance = getAmbientLightIrradiance(uAmbientLight);
    ReflectIndirectBlinnPhong(irradiance, geometry, material, reflectedLight);

    vec3 outLight = reflectedLight.directDiffuse + reflectedLight.directSpecular + reflectedLight.indirectDiffuse;
    FragColor = vec4(outLight, diffuseColor.a);
    FragColor = LinearTosRGB(FragColor);
    // FragColor = vec4(testColor, 1.0);
}
