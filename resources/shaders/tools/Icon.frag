#define IMAGE_QUANTITY 7.

layout (location = 0) in vec2 texCoords;

layout (set = 1, binding = 0) uniform sampler2D iconSampler;
layout (location = 0) out vec4 finalColor;
const vec2 ICONS_TEXTURE_SIZE = vec2(3584, 512);
layout(push_constant) uniform Push {
    vec3 translation;
    vec3 iconColor;
    float imageIndex;
    bool isSelected;
    float iconScale;
} push;

void main() {
    float color = texture(iconSampler, vec2(texCoords.x / IMAGE_QUANTITY + push.imageIndex * ICONS_TEXTURE_SIZE.y / ICONS_TEXTURE_SIZE.x, 1. - texCoords.y)).a;
    if (color <= .1) discard;

    if (push.isSelected){
        finalColor = vec4(1., .5, 0., 1.);
    } else {
        finalColor = vec4(push.iconColor, 1.);
    }
}