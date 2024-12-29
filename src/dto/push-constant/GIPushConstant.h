#ifndef GIPUSHCONSTANT_H
#define GIPUSHCONSTANT_H

namespace Metal {
    struct GIPushConstant final {
      alignas(4) float bias;
    };
}

#endif //GIPUSHCONSTANT_H
