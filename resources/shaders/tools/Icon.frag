#include "../GlobalDataBuffer.glsl"
#include "../CreateRay.glsl"

#define LIGHTS_SET 1
#include "../LightsBuffer.glsl"

layout (location = 0) in vec2 texCoords;
layout (location = 0) out vec4 finalColor;

layout(push_constant) uniform Push {
    vec3 iconColor;
} push;

float sdBoxFrame(vec3 p, vec3 halfSize, float e, in vec3 translation) {
    p = abs(p - translation)-halfSize;
    vec3 q = abs(p+e)-e;
    return min(min(
    length(max(vec3(p.x, q.y, q.z), 0.0))+min(max(p.x, max(q.y, q.z)), 0.0),
    length(max(vec3(q.x, p.y, q.z), 0.0))+min(max(q.x, max(p.y, q.z)), 0.0)),
    length(max(vec3(q.x, q.y, p.z), 0.0))+min(max(q.x, max(q.y, p.z)), 0.0));
}

float sdSphere(vec3 p, float s, in vec3 translation){
    return length(p - translation)-s;
}

bool rayMarch(vec3 ro, vec3 rd, vec3 halfSize, float width, vec3 translation, bool isSphere) {
    float t = 0.0;
    for (int i = 0; i < 256; i++) {
        vec3 p = ro + t * rd;
        float d;
        if (isSphere){
            d = sdSphere(p, width, translation);
        } else {
            d = sdBoxFrame(p, halfSize, width, translation);
        }
        if (d < 0.001) return true;
        if (t > 100.0) break;
        t += d;
    }
    return false;
}

void main(){
    if (globalData.lightsQuantity == 0){
        discard;
    }
    vec3 dir = createRay(texCoords, globalData.invProj, globalData.invView);
    for (uint i = 0; i < globalData.lightsQuantity; i++){
        Light l = lightsBuffer.lights[i];
        if (l.isSphere){
            if (rayMarch(globalData.cameraWorldPosition.xyz, dir, l.max - l.min, computeSphereRadius(l.min, l.max), l.position, true)){
                finalColor = vec4(l.color, 1);
                return;
            }
        } else {
            if (rayMarch(globalData.cameraWorldPosition.xyz, dir, l.max - l.min, .05, l.position, false)){
                finalColor = vec4(push.iconColor, 1);
                return;
            }
        }
    }
    discard;
}