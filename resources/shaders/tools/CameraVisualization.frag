layout (push_constant) uniform Push {
    mat4 model;
    vec4 color;
    float fov;
    float zNear;
    float zFar;
    float aspectRatio;
} push;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = push.color;
}
