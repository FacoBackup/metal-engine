#ifndef GLOBAL_DATA_V
#define GLOBAL_DATA_V
layout (set = 0, binding = 0) uniform GlobalDataBlock {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 projView;
    mat4 invView;
    mat4 invProj;
    mat4 previousProjView;
    vec3 cameraWorldPosition;

    uint lightsCount;
    uint debugFlag;

    uint pathTracerAccumulationCount;
    uint globalFrameCount;
    uint pathTracerMaxSamples;
    uint denoiserEnabled;

} globalData;

#endif