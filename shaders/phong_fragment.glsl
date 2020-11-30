
#define PI 3.141592653589793
#define RECIPROCAL_PI 0.3183098861837907
#define RreflectDirectFunc      RreflectDirectBlinnPhong

#ifdef USE_DIFFUSE_TEXTURE
uniform sampler2D uDiffuseTexture;
#endif
#ifdef USE_SPECULAR_TEXTURE
uniform sampler2D uSpecularTexture;
#endif

uniform vec3 uSpecular;
uniform float uShininess;

in vec2 vUV;
in vec3 vNormal;
in vec3 vViewPosition;
out vec4 FragColor;

vec3 testColor;

struct IncidentLight {
    vec3 color;
    vec3 direction;
    bool visible;
};
struct ReflectedLight {
    vec3 directDiffuse;
    vec3 directSpecular;
    vec3 indirectDiffuse;
    vec3 indirectSpecular;
};
struct GeometricContext {
    vec3 position;
    vec3 normal;
    vec3 viewDir;
//    #ifdef CLEARCOAT
//        vec3 clearcoatNormal;
//    #endif
};

struct BlinnPhongMaterial {
    vec3 diffuseColor;
    vec3 specularColor;
    float specularShininess;
    float specularStrength;
};

#ifndef saturate
    #define saturate(a) clamp( a, 0.0, 1.0 )
#endif

vec3 BRDF_Diffuse_Lambert(const in vec3 diffuseColor) {
    return RECIPROCAL_PI * diffuseColor;
}

float G_BlinnPhong_Implicit() {
    // 几何遮蔽, 描述了光在粗糙物体上的反射损失
    // 有公式的, 不过这里直接取了中间值, 公式的取值范围大致是 (1/8, 1/2)
    return 0.25;
}

vec3 F_Schlick( const in vec3 specularColor, const in float dotLH ) {
    // Original approximation by Christophe Schlick '94
	// float fresnel = pow( 1.0 - dotLH, 5.0 );

	// Optimized variant (presented by Epic at SIGGRAPH '13)
	// https://cdn2.unrealengine.com/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf
    float fresnel = exp2( ( -5.55473 * dotLH - 6.98316 ) * dotLH );
    return (1.0 - specularColor) * fresnel + specularColor;
}

// http://www.thetenthplanet.de/archives/255
// ShaderX7: 2.5: An Efficient and Physically Plausible Real-Time Shading Model
float D_BlinnPhong( const in float shininess, const in float dotNH ) {
    return RECIPROCAL_PI * ( shininess * 0.5 + 1.0 ) * pow( dotNH, shininess );
}

// CookTorranceSimplified
// F: Fresnel, G: geometric self-occlusion, D: normalized Blinn-phong distribution
// see:
// http://www.thetenthplanet.de/archives/255
// ShaderX7: 2.5: An Efficient and Physically Plausible Real-Time Shading Model
vec3 BRDF_Specular_BlinnPhong(const in IncidentLight incidentLight, const in GeometricContext geometry, const in vec3 specularColor, const in float shininess) {
    vec3 halfDir = normalize(incidentLight.direction + geometry.viewDir);
    float dotNH = saturate(dot(geometry.normal, halfDir));
    float dotLH = saturate(dot(incidentLight.direction, halfDir));
    vec3 F = F_Schlick(specularColor, dotLH);
    float G = G_BlinnPhong_Implicit();
    float D = D_BlinnPhong(shininess, dotNH);
    // testColor = vec3(D);
    return F * (G * D);
}

void RreflectDirectBlinnPhong(
    const in IncidentLight directLight,
    const in GeometricContext geometry,
    const in BlinnPhongMaterial material,
    inout ReflectedLight reflectedLight ) {
    float dotNL = saturate(dot(geometry.normal, directLight.direction));
    vec3 irradiance = dotNL * directLight.color;
    #ifndef PHYSICALLY_CORRECT_LIGHTS
        irradiance *= PI;
    #endif
    reflectedLight.directDiffuse +=
        irradiance * BRDF_Diffuse_Lambert( material.diffuseColor );
    reflectedLight.directSpecular += 
        irradiance * BRDF_Specular_BlinnPhong(directLight, geometry, material.specularColor, material.specularShininess) * material.specularStrength;
}

#if DIRECTION_LIGHT_NUM > 0
struct DirectionalLight {
    vec3 direction;
    vec3 color;
};
uniform DirectionalLight directionalLights[DIRECTION_LIGHT_NUM];
void getDirectionalDirectLightIrradiance( const in DirectionalLight directionalLight, const in GeometricContext geometry, out IncidentLight directLight ) {
    directLight.color = directionalLight.color;
    directLight.direction = -normalize(directionalLight.direction);
    directLight.visible = true;
}
#endif


void main()
{
#ifdef USE_DIFFUSE_TEXTURE
    vec4 diffuseColor = texture(uDiffuseTexture, vUV);
#else
    vec4 diffuseColor = vec4(0.5, 0.5, 0.5, 1.0);
#endif

#ifdef USE_SPECULAR_TEXTURE
    float specularStrength = texture2D(uSpecularTexture, vUV);
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
    material.specularShininess = 8;//uShininess;
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

    vec3 outLight = reflectedLight.directDiffuse + reflectedLight.directSpecular;

    FragColor = vec4(outLight, diffuseColor.a);
    // FragColor = vec4(reflectedLight.directSpecular, diffuseColor.a);
    // FragColor = vec4(testColor, diffuseColor.a);
}
