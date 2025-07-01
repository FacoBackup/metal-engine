#ifndef COMPONENTTYPE_H
#define COMPONENTTYPE_H
#include "../../common/enum/Icons.h"

namespace Metal::ComponentTypes {
    enum ComponentType {
        MESH,
        TRANSFORM,
        LIGHT,
        CAMERA,
        VOLUME
    };

    static constexpr const char *NAMES = "Add Entity\0Mesh\0Light\0Volume\0Camera\0";

    static ComponentType ValueOfIndex(const int option) {
        if (option == 1) {
            return ComponentType::MESH;
        }
        if (option == 2) {
            return LIGHT;
        }
        if (option == 4) {
            return CAMERA;
        }
        return VOLUME;
    }

    static const char *NameOf(const ComponentType mode) {
        if (mode == MESH)
            return "Mesh Component";
        if (mode == TRANSFORM)
            return "Transformation Component";
        if (mode == LIGHT)
            return "Light Component";
        if (mode == VOLUME)
            return "Volume Component";
        if (mode == CAMERA)
            return "Camera Component";
        return nullptr;
    }

    static const char *IconOf(const ComponentType mode) {
        if (mode == MESH)
            return Icons::view_in_ar.c_str();
        if (mode == TRANSFORM)
            return Icons::transform.c_str();
        if (mode == LIGHT)
            return Icons::lightbulb.c_str();
        if (mode == VOLUME)
            return Icons::blur_on.c_str();
        if (mode == CAMERA)
            return Icons::camera.c_str();
        return nullptr;
    }
}
#endif //COMPONENTTYPE_H
