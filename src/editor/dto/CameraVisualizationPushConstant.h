#ifndef CAMERAVISUALIZATIONPUSHCONSTANT_H
#define CAMERAVISUALIZATIONPUSHCONSTANT_H

#include <glm/glm.hpp>

namespace Metal {
    struct CameraVisualizationPushConstant {
        glm::mat4 model;
        glm::vec4 color;
        float fov;
        float zNear;
        float zFar;
        float aspectRatio;
    };
}

#endif // CAMERAVISUALIZATIONPUSHCONSTANT_H
