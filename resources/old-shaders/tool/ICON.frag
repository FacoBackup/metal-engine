#define IMAGE_QUANTITY 7.

layout (location = 0) in vec2 texCoords;

layout (binding = 0) uniform sampler2D iconSampler;
layout (location = 0) out vec4 finalColor;


layout(push_constant) uniform Push {
    vec3 translation;
    vec3 iconColor;
    float imageIndex;
    bool isSelected;
} push;

void main() {
    vec2 imageSize = vec2(textureSize(iconSampler, 0));
    float color = texture(iconSampler, vec2(texCoords.x / IMAGE_QUANTITY + push.imageIndex * imageSize.y / imageSize.x, 1. - texCoords.y)).a;
    if (color <= .1) discard;

    if (push.isSelected){
        finalColor = vec4(1., .5, 0., 1.);
    } else {
        finalColor = vec4(push.iconColor, 1.);
    }
}