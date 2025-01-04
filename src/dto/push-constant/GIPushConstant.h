#ifndef GIPUSHCONSTANT_H
#define GIPUSHCONSTANT_H

namespace Metal {
    struct GIPushConstant final {
      alignas(4) float ditheringIntensity;
      alignas(4) unsigned int giBounces;
      alignas(4) unsigned int giSamplesPerPixel;
    };
}

#endif //GIPUSHCONSTANT_H
