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

    uint debugFlag;
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
vec2 hashWorldSpaceCoord(vec3 world){
//    vec3 minBounds = vec3(-100);
//    vec3 maxBounds = vec3(100);
//    vec3 normalizedPos = (world - minBounds) / (maxBounds - minBounds);
//    vec2 uv = vec2(normalizedPos.x  +   .5  *normalizedPos.z, normalizedPos.y  + .5 * normalizedPos.z);
//    return uv;
//        return vec2(rand(world * MAGIC_NUMER_1), rand(world * MAGIC_NUMBER_2));

    // Texture dimensions
    int width = 1280;
    int height = 720;

    // Prime numbers for hashing
    int p1 = 10007;
    int p2 = 10009;
    int p3 = 10037;
    const float  GRID_SIZE = 7;
    // Compute UV coordinates
    vec2 uv = spatialHashToUV(int(round(world.x * GRID_SIZE)), int(round(world.y * GRID_SIZE)), int(round(world.z * GRID_SIZE)), width, height, p1, p2, p3);

    return uv;
}