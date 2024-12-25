vec3 createRay(in vec2 texCoords) {
    // Adjust texCoords for Vulkan's coordinate system
    vec2 pxNDS = vec2(texCoords.x, texCoords.y) * 2.0 - 1.0;
    vec3 pointNDS = vec3(pxNDS, -1.0);
    vec4 pointNDSH = vec4(pointNDS, 1.0);

    // Transform from NDC to eye space
    vec4 dirEye = globalData.invProj * pointNDSH;
    dirEye.w = 0.0;

    // Transform from eye space to world space
    vec3 dirWorld = (globalData.invView * dirEye).xyz;
    return normalize(dirWorld);
}