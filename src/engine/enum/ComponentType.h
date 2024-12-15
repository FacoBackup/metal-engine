#ifndef COMPONENTTYPE_H
#define COMPONENTTYPE_H

namespace Metal::ComponentTypes {
    enum class ComponentType {
        MESH,
        TRANSFORM
    };

    static ComponentType ValueOfIndex(const int option) {
        return ComponentType::MESH;
    }

    static int IndexOfValue(const ComponentType mode) {
        return 0;
    }
}
#endif //COMPONENTTYPE_H
