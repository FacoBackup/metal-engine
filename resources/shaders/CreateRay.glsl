#ifndef CREATE_RAY
#define CREATE_RAY
vec3 createRay(in vec2 texCoords, in mat4 invProjectionMatrix, in mat4 invViewMatrix) {
    vec2 pxNDS = texCoords * 2. - 1.;
    vec4 pointNDSH = vec4(pxNDS, 0.0, 1.0); // Ensure point is at far plane
    vec4 dirEye = invProjectionMatrix * pointNDSH;
    dirEye.w = 0.0;
    vec3 dirWorld = normalize((invViewMatrix * dirEye).xyz);
    return dirWorld;
}
#endif