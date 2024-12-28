vec3 createRay(in vec2 texCoords, in mat4 invProjectionMatrix, in mat4 invViewMatrix) {
    vec2 pxNDS = texCoords * 2. - 1.;
    vec3 pointNDS = vec3(pxNDS, 0);
    vec4 pointNDSH = vec4(pointNDS, 1.0);
    vec4 dirEye = invProjectionMatrix * pointNDSH;
    dirEye.w = 0.;
    vec3 dirWorld = (invViewMatrix * dirEye).xyz;
    return normalize(dirWorld);
}