#ifndef MATERIALDATA_H
#define MATERIALDATA_H
#include <string>

namespace Metal {
    struct MaterialData final {
        std::string albedo;
        std::string normal;
        std::string roughness;
        std::string metallic;
        std::string ao;
        std::string height;
    };
}

#endif //MATERIALDATA_H
