#ifndef HWRAYTRACING_PUSHCONSTANT_H
#define HWRAYTRACING_PUSHCONSTANT_H

namespace Metal {
    struct PathTracerPushConstant {
        alignas(4) float pathTracerMultiplier{};
        alignas(4) unsigned int isAtmosphereEnabled{};

        alignas(4) unsigned int pathTracerSamples{};
        alignas(4) unsigned int pathTracerBounces{};
        alignas(4) float pathTracingEmissiveFactor{};
        alignas(4) unsigned int restirFrameIndex{};
        alignas(4) unsigned int restirEnabled{};
    };
}

#endif //HWRAYTRACING_PUSHCONSTANT_H
