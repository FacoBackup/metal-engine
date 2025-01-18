struct Light {
    vec3 color;
    vec3 position;
    vec3 min;
    vec3 max;
    bool isSphere;
};

layout(set = LIGHTS_SET, binding = 0) uniform Lights {
    Light lights[MAX_LIGHTS];
} lightsBuffer;

float computeSphereRadius(vec3 boxMin, vec3 boxMax) {
    // Compute the diagonal length of the box
    float diagonalLength = length(boxMax - boxMin);
    // Radius is half the diagonal length
    return 0.5 * diagonalLength;
}
