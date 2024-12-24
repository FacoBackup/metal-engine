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

        template<class Archive>
        void save(Archive &ar) const {
            ar(albedo, normal, roughness, metallic, ao);
        }

        template<class Archive>
        void load(Archive &ar) {
            ar(albedo, normal, roughness, metallic, ao);
        }
    };
}

#endif //MATERIALDATA_H
