#ifndef HWRAYTRACING_PUSHCONSTANT_H
#define HWRAYTRACING_PUSHCONSTANT_H

namespace Metal {
    struct HWRayTracingPushConstant {
        alignas(4) float pathTracerMultiplier{};
        alignas(4) unsigned int volumeShadowSteps = 0;
        alignas(4) unsigned int isAtmosphereEnabled{};

        alignas(4) unsigned int pathTracerSamples{};
        alignas(4) unsigned int pathTracerBounces{};
        alignas(4) float pathTracingEmissiveFactor{};
        alignas(4) unsigned int shouldTrace{};

        alignas(4) unsigned int dofEnabled{};
        alignas(4) float dofFocusDistance{};
        alignas(4) float dofAperture{};
        alignas(4) float dofFocalLength{};
    };
}

#endif //HWRAYTRACING_PUSHCONSTANT_H
