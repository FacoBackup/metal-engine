#ifndef LIGHTTYPE_H
#define LIGHTTYPE_H
#include <unordered_map>

namespace Metal::LightVolumeTypes{
    enum LightVolumeType {
        SPHERE,
        PLANE,
        VOLUME
    };

    inline std::unordered_map<std::string, unsigned int> getEntries() {
        std::unordered_map<std::string, unsigned int> entries;
        entries["ITEM_TYPE_SPHERE"] = SPHERE;
        entries["ITEM_TYPE_PLANE"] = PLANE;
        entries["ITEM_TYPE_VOLUME"] = VOLUME;
        return entries;
    }
}
#endif //LIGHTTYPE_H
