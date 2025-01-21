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

float computeSphereRadius(const in Light l) {
    // Compute the diagonal length of the box
    float diagonalLength = length(l.max - l.min);
    // Radius is half the diagonal length
    return 0.5 * diagonalLength;
}
