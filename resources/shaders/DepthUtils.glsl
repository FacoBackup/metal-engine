float getLogDepth(vec2 uv, sampler2D sceneDepth, float logDepthFC) {
    return pow(2.0, texture(sceneDepth, uv).r / (logDepthFC * .5));
}

float getLogDepthFromSampler(vec4 data, float logDepthFC) {
    float half_co = logDepthFC * .5;
    float exponent = data.r / half_co;
    return pow(2.0, exponent);
}

vec3 viewSpacePositionFromDepth(float depth, vec2 texCoords, in mat4 invProjectionMatrix) {
    vec4 clipSpacePosition = vec4(texCoords * 2.0 - 1.0, depth, 1.0);
    vec4 viewSpacePosition = invProjectionMatrix * clipSpacePosition;
    viewSpacePosition /= viewSpacePosition.w;
    return viewSpacePosition.rgb;
}

vec3 worldSpacePositionFromDepth(float depth, vec2 texCoords, in mat4 invProjectionMatrix, in mat4 invView) {
    vec3 viewSpacePosition = viewSpacePositionFromDepth(depth, texCoords, invProjectionMatrix);
    return vec3(invView * vec4(viewSpacePosition, 1.));
}

vec3 normalFromDepth(sampler2D depthSampler, float depth, vec2 texCoords, vec2 bufferResolution, in mat4 invProjectionMatrix, float logDepthFC) {
    vec2 texelSize = 1. / bufferResolution;
    vec2 texCoords1 = texCoords + vec2(0., 1.) * texelSize;
    vec2 texCoords2 = texCoords + vec2(1., 0.) * texelSize;

    float depth1 = getLogDepth(texCoords1, depthSampler, logDepthFC);
    float depth2 = getLogDepth(texCoords2, depthSampler, logDepthFC);

    vec3 P0 = viewSpacePositionFromDepth(depth, texCoords, invProjectionMatrix).xyz;
    vec3 P1 = viewSpacePositionFromDepth(depth1, texCoords1, invProjectionMatrix).xyz;
    vec3 P2 = viewSpacePositionFromDepth(depth2, texCoords2, invProjectionMatrix).xyz;

    return normalize(cross(P2 - P0, P1 - P0));
}