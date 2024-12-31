#ifndef COMPONENTTYPE_H
#define COMPONENTTYPE_H
#include "../common/interface/Icons.h"

namespace Metal::ComponentTypes {
    enum ComponentType {
        MESH,
        TRANSFORM,
        LIGHT
    };

    static constexpr const char *NAMES = "Add Entity\0Mesh\0Light\0";

    static ComponentType ValueOfIndex(const int option) {
        if (option == 1) {
            return ComponentType::MESH;
        }
        return LIGHT;
    }

    static const char *NameOf(const ComponentType mode) {
        if (mode == MESH)
            return "Mesh Component";
        if (mode == TRANSFORM)
            return "Transformation Component";
        if (mode == LIGHT)
            return "Light Component";
        return nullptr;
    }

    static const char *IconOf(const ComponentType mode) {
        if (mode == MESH)
            return Icons::view_in_ar.c_str();
        if (mode == TRANSFORM)
            return Icons::transform.c_str();
        if (mode == LIGHT)
            return Icons::lightbulb.c_str();
        return nullptr;
    }
}
#endif //COMPONENTTYPE_H
