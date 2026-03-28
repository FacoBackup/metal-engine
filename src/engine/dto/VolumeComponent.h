#ifndef VOLUMECOMPONENT_H
#define VOLUMECOMPONENT_H

#include "AbstractComponent.h"

#include "../../common/Reflection.h"

#include <glm/vec3.hpp>

namespace Metal {
    struct VolumeComponent final : AbstractComponent {
        glm::vec3 albedo = glm::vec3(1.0f);
        float density = 1;
        float g = 0;
        float scatteringAlbedo = 5;
        int samples = 64;

    protected:
        void registerFields() override;

    public:
        ComponentType getType() const override;
    };
} // Metal


#endif //VOLUMECOMPONENT_H
