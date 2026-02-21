#ifndef COMPONENTTYPE_H
#define COMPONENTTYPE_H
#include "../common/interface/Icons.h"

namespace Metal::ComponentTypes {
    enum ComponentType {
        MESH,
        TRANSFORM,
        SPHERE_LIGHT,
        PLANE_LIGHT,
        VOLUME
    };

    static constexpr const char *NAMES = "Add Entity\0Mesh\0Sphere Light\0Plane Light\0Volume\0";

    static ComponentType ValueOfIndex(const int option) {
        if (option == 1) {
            return MESH;
        }
        if (option == 2) {
            return SPHERE_LIGHT;
        }
        if (option == 3) {
            return PLANE_LIGHT;
        }
        return VOLUME;
    }

    static const char *NameOf(const ComponentType mode) {
        if (mode == MESH)
            return "Mesh Component";
        if (mode == TRANSFORM)
            return "Transformation Component";
        if (mode == SPHERE_LIGHT)
            return "Sphere Light Component";
        if (mode == PLANE_LIGHT)
            return "Plane Light Component";
        if (mode == VOLUME)
            return "Volume Component";
        return nullptr;
    }

    static const char *IconOf(const ComponentType mode) {
        if (mode == MESH)
            return Icons::view_in_ar.c_str();
        if (mode == TRANSFORM)
            return Icons::transform.c_str();
        if (mode == SPHERE_LIGHT || mode == PLANE_LIGHT)
            return Icons::lightbulb.c_str();
        if (mode == VOLUME)
            return Icons::blur_on.c_str();
        return nullptr;
    }
}
#endif //COMPONENTTYPE_H
