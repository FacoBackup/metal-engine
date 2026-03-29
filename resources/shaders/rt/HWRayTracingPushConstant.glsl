#ifndef HW_RAY_TRACING_PUSH_CONSTANT_GLSL
#define HW_RAY_TRACING_PUSH_CONSTANT_GLSL

layout(push_constant) uniform PushConstants {
    float pathTracerMultiplier;
    uint isAtmosphereEnabled;

    uint pathTracerSamples;
    uint pathTracerBounces;
    float pathTracingEmissiveFactor;
} pushConstants;

#endif
