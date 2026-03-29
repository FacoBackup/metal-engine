#ifndef GLOBAL_DATA_V
#define GLOBAL_DATA_V
layout (set = 0, binding = 0, std140) uniform GlobalDataBlock {
    mat4 projView;
    mat4 previousProjView;
    mat4 invView;
    mat4 invProj;
    mat4 invProjView;
    vec3 cameraWorldPosition;
    uint lightsCount;
    vec3 sunPosition;
    uint debugFlag;

    uint pathTracerAccumulationCount;
    uint globalFrameCount;
    uint pathTracerMaxSamples;
    uint denoiserEnabled;
    uint isOrthographic;

} globalData;

bool intersectBox(vec3 ro, vec3 rd, vec3 dimensions, out float tEntry, out float tExit) {
    vec3 boxMin = -dimensions * 0.5;
    vec3 boxMax = dimensions * 0.5;
    vec3 invRd = 1.0 / rd;

    vec3 t0s = (boxMin - ro) * invRd;
    vec3 t1s = (boxMax - ro) * invRd;
    vec3 tsmaller = min(t0s, t1s);
    vec3 tbigger = max(t0s, t1s);

    tEntry = max(max(tsmaller.x, tsmaller.y), tsmaller.z);
    tExit = min(min(tbigger.x, tbigger.y), tbigger.z);

    return (tExit >= max(tEntry, 0.0));
}
#endif