#include "Camera.h"

namespace Metal {
    void Camera::extractFrustumPlanes(glm::mat4x4 m) {
        for (int i = 0; i < 6; i++) {
            extractPlane(m, i, planes[i]);
        }
    }

    bool Camera::isSphereInsideFrustum(const glm::vec3 center, const float radius) const {
        for (const auto plane: planes) {
            if (const float distance = plane.x * center.x + plane.y * center.y + plane.z * center.z + plane.w;
                distance < -radius) {
                return false;
            }
        }
        return true;
    }

    void Camera::extractPlane(const glm::mat4 &matrix, const int index, glm::vec4 &plane) {
        switch (index) {
            case 0: // Left
                plane = matrix[3] + matrix[0];
                break;
            case 1: // Right
                plane = matrix[3] - matrix[0];
                break;
            case 2: // Bottom
                plane = matrix[3] + matrix[1];
                break;
            case 3: // Top
                plane = matrix[3] - matrix[1];
                break;
            case 4: // Near
                plane = matrix[3] + matrix[2];
                break;
            case 5: // Far
                plane = matrix[3] - matrix[2];
                break;
            default: plane = glm::vec4{0, 0, 0, 0};
        }
        // Normalize the plane
        plane /= glm::length(glm::vec3(plane));
    }
}
