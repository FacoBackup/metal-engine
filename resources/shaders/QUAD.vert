#version 460
const vec3 positions[4] = vec3[4](vec3(-1, -1, -4.371138828673793e-8), vec3(1, -1, -4.371138828673793e-8), vec3(-1, 1, 4.371138828673793e-8F), vec3(1, 1, 4.371138828673793e-8F));

out vec2 texCoords;
void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 1.0);
    texCoords = positions[gl_VertexIndex].xy * 0.5 + 0.5;
}