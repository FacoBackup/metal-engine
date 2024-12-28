#include "../GlobalDataBuffer.glsl"

layout(location = 0) out vec2 texCoord;

layout(push_constant) uniform Push {
    vec3 translation;
    vec3 iconColor;
    float imageIndex;
    bool isSelected;
    float iconScale;

} push;

const vec2 positions[4] = vec2[](
    vec2(-1.0, -1.0),
    vec2(1.0, -1.0),
    vec2(-1.0, 1.0),
    vec2(1.0, 1.0)
);

const vec2 texCoords[4] = vec2[](
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(0.0, 1.0),
    vec2(1.0, 1.0)
);

void main() {
    mat4 m = mat4(1);
    m[3] = vec4(push.translation, 1);
    m = globalData.viewMatrix * m;
    m[0][0] = push.iconScale;
    m[1][1] = push.iconScale;
    m[2][2] = push.iconScale;

    m[0][1] = 0.0;
    m[0][2] = 0.0;
    m[0][3] = 0.0;
    m[1][0] = 0.0;
    m[1][2] = 0.0;
    m[1][3] = 0.0;
    m[2][0] = 0.0;
    m[2][1] = 0.0;
    m[2][3] = 0.0;


    gl_Position = globalData.projectionMatrix * m * vec4(positions[gl_VertexIndex], 0.0, 1.0);
    texCoord = texCoords[gl_VertexIndex];
}