#ifndef COMPONENTTYPE_H
#define COMPONENTTYPE_H
#include "../common/interface/Icons.h"

namespace Metal::ComponentTypes {
    enum ComponentType {
        PRIMITIVE,
        TRANSFORM,
        SPHERE_LIGHT,
        PLANE_LIGHT,
        VOLUME,
        ATMOSPHERE
    };

    static constexpr const char *NAMES = "Add Entity\0Mesh\0Sphere Light\0Plane Light\0Volume\0Atmosphere\0";

    static ComponentType ValueOfIndex(const int option) {
        if (option == 1) {
            return PRIMITIVE;
        }
        if (option == 2) {
            return SPHERE_LIGHT;
        }
        if (option == 3) {
            return PLANE_LIGHT;
        }
        if (option == 4) {
            return VOLUME;
        }
        return ATMOSPHERE;
    }

    static const char *NameOf(const ComponentType mode) {
        if (mode == PRIMITIVE)
            return "Mesh Component";
        if (mode == TRANSFORM)
            return "Transformation Component";
        if (mode == SPHERE_LIGHT)
            return "Sphere Light Component";
        if (mode == PLANE_LIGHT)
            return "Plane Light Component";
        if (mode == VOLUME)
            return "Volume Component";
        if (mode == ATMOSPHERE)
            return "Atmosphere Component";
        return nullptr;
    }

    static const char *IconOf(const ComponentType mode) {
        if (mode == PRIMITIVE)
            return Icons::view_in_ar.c_str();
        if (mode == TRANSFORM)
            return Icons::transform.c_str();
        if (mode == SPHERE_LIGHT || mode == PLANE_LIGHT)
            return Icons::lightbulb.c_str();
        if (mode == VOLUME)
            return Icons::blur_on.c_str();
        if (mode == ATMOSPHERE)
            return Icons::cloud.c_str();
        return nullptr;
    }
}
#endif //COMPONENTTYPE_H
