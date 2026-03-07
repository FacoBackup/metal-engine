layout (location = 0) out float outID;

layout (push_constant) uniform Push {
    mat4 model;
    vec4 selectionColor;
    uint renderIndex;
} push;

void main() {
    outID = float(push.renderIndex + 1);
}
