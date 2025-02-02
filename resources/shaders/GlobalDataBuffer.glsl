layout(set = 0, binding = 0) uniform GlobalDataBlock {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 projView;
    mat4 invView;
    mat4 invProj;
    vec3 cameraWorldPosition;
    vec3 sunColor;
    vec3 sunPosition;
    float giStrength;
    uint lightCount;
    uint volumeCount;
    bool isAtmosphereEnabled;

// GI
    bool enabledDenoiser;
    bool multipleImportanceSampling;
    uint giMaxAccumulation;
    uint giSamples;
    uint giBounces;
    uint giTileSubdivision;
    float giEmissiveFactor;

    uint debugFlag;

    uint surfaceCacheWidth;
    uint surfaceCacheHeight;

    uint giAccumulationCount;
    uint globalFrameCount;
} globalData;


#define MAGIC_NUMBER1 23.53412364564
#define MAGIC_NUMBER2 12.1232134312543

float rand(vec3 co) {
    return fract(sin(dot(co, vec3(12.9898, 71.9898, 78.233))) * 43758.5453);
}
vec2 coordToSpatialHash(ivec3 coord, uint width, uint height) {
    const int p1 = 39937;
    const int p2 = 37199;
    const int p3 = 39119;
    int signedX =coord.x >= 0 ? coord.x * 2 : (-coord.x * 2 - 1);
    int signedY =coord.y >= 0 ? coord.y * 2 : (-coord.y * 2 - 1);
    int signedZ =coord.z >= 0 ? coord.z * 2 : (-coord.z * 2 - 1);

    // Compute the spatial hash
    uint hashValue = ((signedX * p1) ^ (signedY * p2) ^ (signedZ * p3)) % (width * height);

    // Compute UV coordinates
    float u = float(hashValue % width) / float(width);
    float v = float((hashValue / width) % height) / float(height);
    return vec2(u, v);
}

vec2 genHashSurfaceCache(vec3 world){
    float gridSize = float(globalData.giTileSubdivision);
    ivec3 coord = ivec3(int(round(world.x * gridSize)), int(round(world.y * gridSize)), int(round(world.z * gridSize)));
    return coordToSpatialHash(coord, globalData.surfaceCacheWidth, globalData.surfaceCacheHeight);
}

vec3 worldToCacheSpace(in vec3 worldSpacePos){
    float voxelSize = float(globalData.giTileSubdivision);
    return (round(worldSpacePos * voxelSize) / voxelSize);
}

vec2 worldToSurfaceCacheHash(in vec3 worldSpacePos){
    vec3 cacheSpace = worldToCacheSpace(worldSpacePos);
    return genHashSurfaceCache(cacheSpace);
}