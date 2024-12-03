#ifndef FRUSTUM_H
#define FRUSTUM_H
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

namespace Metal {
    class Frustum {
    public:
        void extractFrustumPlanes(glm::mat4x4 m);

        bool isSphereInsideFrustum(glm::vec3 center, float radius) const;

    private:
        static void extractPlane(const glm::mat4 &matrix, int index, glm::vec4 &plane);

        glm::vec4 planes[6] = {glm::vec4{}, glm::vec4{}, glm::vec4{}, glm::vec4{}, glm::vec4{}, glm::vec4{}};
    };
}

#endif
