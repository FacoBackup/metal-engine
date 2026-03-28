#ifndef GRID_UTIL_GLSL
#define GRID_UTIL_GLSL

struct GridData {
    vec3 baseColor;
    float roughness;
    float metallic;
};

GridData getDebugGrid(vec3 worldPos) {
    vec3 pos = worldPos * 2.0;
    vec3 f = abs(fract(pos) - 0.5);
    vec3 g = step(0.48, f);
    float grid = max(max(g.x, g.y), g.z);

    GridData data;
    data.baseColor = mix(vec3(0.2), vec3(0.7), grid);
    data.roughness = mix(0.9, 0.1, grid);
    data.metallic = 0.0;
    return data;
}

#endif
