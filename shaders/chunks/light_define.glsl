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

vec4 LinearTosRGB( in vec4 value ) {
    return vec4( mix( pow( value.rgb, vec3( 0.41666 ) ) * 1.055 - vec3( 0.055 ), value.rgb * 12.92, vec3( lessThanEqual( value.rgb, vec3( 0.0031308 ) ) ) ), value.a );
}

float punctualLightIntensityToIrradianceFactor(const in float lightDistance, const in float cutoffDistance, const in float decayExponent) {
#if defined ( PHYSICALLY_CORRECT_LIGHTS )
    float distanceFalloff = 1.0 / max( pow( lightDistance, decayExponent ), 0.01 );
    if( cutoffDistance > 0.0 ) {
        distanceFalloff *= pow2( saturate( 1.0 - pow4( lightDistance / cutoffDistance ) ) );
    }
    return distanceFalloff;
#else
    if( cutoffDistance > 0.0 && decayExponent > 0.0 ) {
        return pow( saturate( -lightDistance / cutoffDistance + 1.0 ), decayExponent );
    }
    return 1.0;
#endif
}

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
    // testColor = material.diffuseColor.rgb;
    reflectedLight.directDiffuse +=
        irradiance * BRDF_Diffuse_Lambert( material.diffuseColor );
    testColor = reflectedLight.directDiffuse;
    reflectedLight.directSpecular += 
        irradiance * BRDF_Specular_BlinnPhong(directLight, geometry, material.specularColor, material.specularShininess) * material.specularStrength;
}

void ReflectIndirectBlinnPhong(
    const in vec3 irradiance,
    const in GeometricContext geometry,
    const in BlinnPhongMaterial material,
    inout ReflectedLight reflectedLight) {
    reflectedLight.indirectDiffuse += irradiance * BRDF_Diffuse_Lambert(material.diffuseColor);
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

#if POINT_LIGHT_NUM > 0
struct PointLight {
    vec3 position;
    vec3 color;
    float distance;
    float decay;
};
uniform PointLight pointLights[POINT_LIGHT_NUM];
void getPointDirectLightIrradiance( const in PointLight pointLight, const in GeometricContext geometry, out IncidentLight directLight ) {
    vec3 lVector = pointLight.position - geometry.position;
    directLight.direction = normalize( lVector );
    float lightDistance = length( lVector );
    directLight.color = pointLight.color;
    directLight.color *= punctualLightIntensityToIrradianceFactor( lightDistance, pointLight.distance, pointLight.decay );
    directLight.visible = (directLight.color != vec3(0.0));
}
#endif

#if SPOT_LIGHT_NUM > 0
struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float distance;
    float decay;
    float coneCos;
    float penumbraCos;
};
uniform SpotLight spotLights[SPOT_LIGHT_NUM];
void getSpotDirectLightIrradiance(const in SpotLight spotLight, const in GeometricContext geometry, out IncidentLight directLight) {
    vec3 lvector = spotLight.position - geometry.position;
    directLight.direction = normalize(lvector);
    float distance = length(lvector);
    float angleCos = dot(-directLight.direction, spotLight.direction);
    if (angleCos > spotLight.coneCos) {
        float spotEffect = smoothstep(spotLight.coneCos, spotLight.penumbraCos, angleCos);
        directLight.color = spotLight.color;
        directLight.color *= spotEffect * punctualLightIntensityToIrradianceFactor(distance, spotLight.distance, spotLight.decay);
        directLight.visible = true;
    } else {
        directLight.color = vec3(0.0);
        directLight.visible = false;
    }
}
#endif

vec3 getAmbientLightIrradiance(const in vec3 ambientLightColor) {
    vec3 irradiance = ambientLightColor;
    #ifndef PHYSICALLY_CORRECT_LIGHTS
        irradiance *= PI;
    #endif
    return irradiance;
}

struct ToonMaterial {
    vec3 diffuseColor;
};

vec3 getGradientIrradiance(vec3 normal, vec3 lightDirection) {
    float dotNL = dot( normal, lightDirection );
    vec2 coord = vec2( dotNL * 0.5 + 0.5, 0.0 );
    #ifdef USE_GRADIENTMAP
        return texture2D( gradientMap, coord ).rgb;
    #else
        return ( coord.x < 0.7 ) ? vec3( 0.7 ) : vec3( 1.0 );
    #endif
}

// TOON SHADER
void RE_Direct_Toon( const in IncidentLight directLight, const in GeometricContext geometry, const in ToonMaterial material, inout ReflectedLight reflectedLight ) {
    vec3 irradiance = getGradientIrradiance( geometry.normal, directLight.direction ) * directLight.color;
    #ifndef PHYSICALLY_CORRECT_LIGHTS
        irradiance *= PI;
    #endif
    reflectedLight.directDiffuse += irradiance * BRDF_Diffuse_Lambert( material.diffuseColor );
}
void RE_IndirectDiffuse_Toon( const in vec3 irradiance, const in GeometricContext geometry, const in ToonMaterial material, inout ReflectedLight reflectedLight ) {
    reflectedLight.indirectDiffuse += irradiance * BRDF_Diffuse_Lambert( material.diffuseColor );
}
