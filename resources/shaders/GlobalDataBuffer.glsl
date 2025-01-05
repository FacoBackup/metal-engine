layout(set = 0, binding = 0) uniform GlobalDataBlock {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 projView;
    mat4 invView;
    mat4 invProj;
    vec3 cameraWorldPosition;
    vec3 sunColor;
    vec3 sunPosition;
    float logDepthFC;
    uint lightsQuantity;
    bool enabledSun;

    bool giEnabled;
// POST PROCESSING
    float distortionIntensity;
    float chromaticAberrationIntensity;
    bool distortionEnabled;
    bool chromaticAberrationEnabled;
    bool vignetteEnabled;
    float vignetteStrength;

// GI
    uint giBounces;
    uint giSamplesPerPixel;
    uint giTileSubdivision;

    uint debugFlag;

    uint giBufferWidth;
    uint giBufferHeight;
    uint frameCount;
} globalData;


#define MAGIC_NUMBER1 23.53412364564
#define MAGIC_NUMBER2 12.1232134312543

float rand(vec3 co) {
    return fract(sin(dot(co, vec3(12.9898, 71.9898, 78.233))) * 43758.5453);
}
vec2 spatialHashToUV(int x, int y, int z, int width, int height, int p1, int p2, int p3) {
    int signedX = x >= 0 ? x * 2 : (-x * 2 - 1);
    int signedY = y >= 0 ? y * 2 : (-y * 2 - 1);
    int signedZ = z >= 0 ? z * 2 : (-z * 2 - 1);

    // Compute the spatial hash
    int hashValue = ((signedX * p1) ^ (signedY * p2) ^ (signedZ * p3)) % (width * height);

    // Compute UV coordinates
    float u = float(hashValue % width) / float(width);
    float v = float((hashValue / width) % height) / float(height);

    return vec2(u, v);
}

//float getLevelOfDetailTile(float distanceFromRayOrigin){
//    if (distanceFromRayOrigin < 10){
//        return 10;
//    }
//    if (distanceFromRayOrigin < 20){
//        return 7;
//    }
//    if (distanceFromRayOrigin < 30){
//        return 5;
//    }
//    return 3;
//}

vec2 hashWorldSpaceCoord(vec3 world){
    int p1 = 10007;
    int p2 = 10009;
    int p3 = 10037;
    //    const float  GRID_SIZE = getLevelOfDetailTile(length(globalData.cameraWorldPosition - world));
    float S = float(globalData.giTileSubdivision);
    return spatialHashToUV(int(round(world.x * S)), int(round(world.y * S)), int(round(world.z * S)), int(globalData.giBufferWidth), int(globalData.giBufferHeight), p1, p2, p3);
}