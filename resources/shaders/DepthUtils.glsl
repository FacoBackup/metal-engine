float getLogDepth(vec2 uv, sampler2D sceneDepth, float logDepthFC) {
    float half_co = logDepthFC * .5;
    float exponent = textureLod(sceneDepth, uv, 2.).r / half_co;
    return pow(2.0, exponent);
}

float getLogDepthFromSampler(vec4 data, float logDepthFC) {
    float half_co = logDepthFC * .5;
    float exponent = data.r / half_co;
    return pow(2.0, exponent);
}

vec3 viewSpacePositionFromDepth(float logarithimicDepth, vec2 texCoords, in mat4 invProjectionMatrix) {
    float z = logarithimicDepth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(texCoords * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invProjectionMatrix * clipSpacePosition;
    viewSpacePosition /= viewSpacePosition.w;

    return viewSpacePosition.rgb;
}

vec3 normalFromDepth(sampler2D depthSampler, float logarithimicDepth, vec2 texCoords, vec2 bufferResolution, in mat4 invProjectionMatrix, float logDepthFC) {
    vec2 texelSize = 1. / bufferResolution;
    vec2 texCoords1 = texCoords + vec2(0., 1.) * texelSize;
    vec2 texCoords2 = texCoords + vec2(1., 0.) * texelSize;

    float depth1 = getLogDepth(texCoords1, depthSampler, logDepthFC);
    float depth2 = getLogDepth(texCoords2, depthSampler, logDepthFC);

    vec3 P0 = viewSpacePositionFromDepth(logarithimicDepth, texCoords, invProjectionMatrix);
    vec3 P1 = viewSpacePositionFromDepth(depth1, texCoords1, invProjectionMatrix);
    vec3 P2 = viewSpacePositionFromDepth(depth2, texCoords2, invProjectionMatrix);

    return normalize(cross(P2 - P0, P1 - P0));
}