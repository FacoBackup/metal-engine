#ifndef COMPONENTTYPE_H
#define COMPONENTTYPE_H
#include "../common/interface/Icons.h"

namespace Metal::ComponentTypes {
    enum ComponentType {
        MESH,
        TRANSFORM
    };

    static ComponentType ValueOfIndex(const int option) {
        return ComponentType::MESH;
    }

    static int IndexOfValue(const ComponentType mode) {
        return 0;
    }

    static const char *NameOf(const ComponentType mode) {
        if (mode == MESH)
            return "Mesh Component";
        if (mode == TRANSFORM)
            return "Transformation Component";
        return nullptr;
    }

    static const char *IconOf(const ComponentType mode) {
        if (mode == MESH)
            return Icons::view_in_ar.c_str();
        if (mode == TRANSFORM)
            return Icons::transform.c_str();
        return nullptr;
    }
}
#endif //COMPONENTTYPE_H
