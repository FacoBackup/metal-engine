#version 460

layout (location = 0) out vec4 finalColor;
layout(location = 0) in vec2 texCoord;

void main () {
    finalColor = vec4(texCoord, 0, 1);
}