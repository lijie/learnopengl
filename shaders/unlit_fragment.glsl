
#define PI 3.141592653589793
#define RECIPROCAL_PI 0.3183098861837907
#define RreflectDirectFunc      RreflectDirectBlinnPhong

#ifdef USE_DIFFUSE_TEXTURE
uniform sampler2D uDiffuseTexture;
#endif

uniform vec3 uAlbedo;

in vec2 vUV;
out vec4 FragColor;

vec3 testColor;
#include chunks/pack.glsl

// 深度纹理一般颜色很浅, 处理一下更容易看清楚.
// see: https://learnopengl.com/Advanced-OpenGL/Depth-testing
float near = 0.1; 
float far  = 100.0; 
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
    vec4 diffuseColor = vec4(uAlbedo, 1.0);
#ifdef USE_DIFFUSE_TEXTURE
#ifdef USE_DEPTH_TEXTURE
    // float depth = unpackRGBAToDepth(texture2D(uDiffuseTexture, vUV));
    float depth = texture(uDiffuseTexture, vUV).r;
    depth = LinearizeDepth(depth) / far;
    diffuseColor = vec4(vec3(depth), 1.0);
#else
    diffuseColor = texture(uDiffuseTexture, vUV);
#endif
#endif

    FragColor = diffuseColor;
}
