#ifndef LIGHTTYPE_H
#define LIGHTTYPE_H
#include <unordered_map>

namespace Metal::LightTypes{
    enum LightType {
        SPHERE,
        PLANE,
        SPOTLIGHT,
        EMISSIVE_SURFACE
    };

    inline std::unordered_map<std::string, unsigned int> getEntries() {
        std::unordered_map<std::string, unsigned int> entries;
        entries["LIGHT_TYPE_SPHERE"] = SPHERE;
        entries["LIGHT_TYPE_PLANE"] = PLANE;
        entries["LIGHT_TYPE_SPOTLIGHT"] = SPOTLIGHT;
        entries["LIGHT_TYPE_EMISSIVE_SURFACE"] = EMISSIVE_SURFACE;
        return entries;
    }
}
#endif //LIGHTTYPE_H
