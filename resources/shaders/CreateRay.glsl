vec3 createRay(in vec2 texCoords, in mat4 invProjectionMatrix, in mat4 invViewMatrix, out vec3 rayOrigin) {
    vec2 pxNDS = texCoords * 2. - 1.;

    if (globalData.isOrthographic != 0) {
        // Orthographic ray generation
        vec4 rayOriginEye = invProjectionMatrix * vec4(pxNDS, -1.0, 1.0); // -1 for near plane
        rayOriginEye.w = 1.0;
        rayOrigin = (invViewMatrix * rayOriginEye).xyz;

        vec4 rayDirEye = vec4(0.0, 0.0, -1.0, 0.0); // View space forward
        return normalize((invViewMatrix * rayDirEye).xyz);
    } else {
        // Perspective ray generation
        rayOrigin = globalData.cameraWorldPosition;
        vec4 pointNDSH = vec4(pxNDS, 0.0, 1.0); // Ensure point is at far plane
        vec4 dirEye = invProjectionMatrix * pointNDSH;
        dirEye.w = 0.0;
        vec3 dirWorld = normalize((invViewMatrix * dirEye).xyz);
        return dirWorld;
    }
}

vec3 createRay(in vec2 texCoords, in mat4 invProjectionMatrix, in mat4 invViewMatrix) {
    vec3 dummyOrigin;
    return createRay(texCoords, invProjectionMatrix, invViewMatrix, dummyOrigin);
}