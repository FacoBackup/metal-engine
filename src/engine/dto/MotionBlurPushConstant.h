#ifndef MOTIONBLURPUSHCONSTANT_H
#define MOTIONBLURPUSHCONSTANT_H

namespace Metal {
    struct MotionBlurPushConstant {
        alignas(4) bool motionBlurEnabled;
        alignas(4) float motionBlurVelocityScale;
        alignas(4) int motionBlurMaxSamples;
    };
}

#endif //MOTIONBLURPUSHCONSTANT_H
