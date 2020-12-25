#include chunks/pack.glsl

in vec2 vHighPrecisionZW;

void main()
{
#ifdef PACK_DEPTH
    float fragCoordZ = 0.5 * vHighPrecisionZW[0] / vHighPrecisionZW[1] + 0.5;
    gl_FragColor = packDepthToRGBA(fragCoordZ);
#else
    // nothing
#endif
}
