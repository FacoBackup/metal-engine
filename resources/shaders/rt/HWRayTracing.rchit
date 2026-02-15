#extension GL_EXT_ray_tracing : require

layout(location = 0) rayPayloadInEXT vec3 hitColor;

void main() {
    // Hit detected - return flat color (white)
    hitColor = vec3(1.0, 1.0, 1.0);
}
