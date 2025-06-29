#ifndef PPSETTINGS_H
#define PPSETTINGS_H

namespace Metal {
    struct PostProcessingPC {
        alignas(4) float distortionIntensity;
        alignas(4) float chromaticAberrationIntensity;
        alignas(4) bool distortionEnabled;
        alignas(4) bool chromaticAberrationEnabled;
        alignas(4) bool vignetteEnabled;
        alignas(4) float vignetteStrength;
    };
}
#endif //PPSETTINGS_H
