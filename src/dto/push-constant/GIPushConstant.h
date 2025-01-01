#ifndef GIPUSHCONSTANT_H
#define GIPUSHCONSTANT_H

namespace Metal {
    struct GIPushConstant final {
      alignas(4) float biasHit;
      alignas(4) float shadowsBaseColor;
      alignas(4) float ditheringIntensity;
    };
}

#endif //GIPUSHCONSTANT_H
