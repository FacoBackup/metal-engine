#ifndef GIPUSHCONSTANT_H
#define GIPUSHCONSTANT_H

namespace Metal {
    struct GIPushConstant final {
      alignas(4) float bias;
      alignas(4) float biasHit;
      alignas(4) float shadowsBaseColor;
    };
}

#endif //GIPUSHCONSTANT_H
