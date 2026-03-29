#ifndef DEPTH_OF_FIELD_PUSH_CONSTANT_H
#define DEPTH_OF_FIELD_PUSH_CONSTANT_H

namespace Metal {
    struct DepthOfFieldPushConstant {
        alignas(4) float focusDistance{};
        alignas(4) float aperture{};
        alignas(4) float focalLength{};
        alignas(4) unsigned int enabled{};
    };
}

#endif //DEPTH_OF_FIELD_PUSH_CONSTANT_H
