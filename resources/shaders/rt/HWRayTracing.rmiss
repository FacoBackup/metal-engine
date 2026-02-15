#extension GL_EXT_ray_tracing : require

layout(location = 0) rayPayloadInEXT vec3 hitColor;

void main() {
    // No hit - return sky gradient based on ray direction
    vec3 dir = normalize(gl_WorldRayDirectionEXT);
    float t = 0.5 * (dir.y + 1.0);
    hitColor = mix(vec3(0.2, 0.3, 0.5), vec3(0.5, 0.7, 1.0), t);
}
