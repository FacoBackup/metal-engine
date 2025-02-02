#include "../GlobalDataBuffer.glsl"
#include "../CreateRay.glsl"

#define LIGHT_VOLUME_SET 1
#include "../LightVolumeBuffer.glsl"

layout (location = 0) in vec2 texCoords;
layout (location = 0) out vec4 finalColor;

layout(push_constant) uniform Push {
    vec3 iconColor;
} push;

float sdSphere(vec3 p, float s, in vec3 translation){
    return length(p - translation)-s;
}
float dot2(vec3 v) {
    return dot(v, v);
}
float udQuad(vec3 p, vec3 a, vec3 b, vec3 c, vec3 d)
{
    vec3 ba = b - a; vec3 pa = p - a;
    vec3 cb = c - b; vec3 pb = p - b;
    vec3 dc = d - c; vec3 pc = p - c;
    vec3 ad = a - d; vec3 pd = p - d;
    vec3 nor = cross(ba, ad);

    return sqrt(
    (sign(dot(cross(ba, nor), pa)) +
    sign(dot(cross(cb, nor), pb)) +
    sign(dot(cross(dc, nor), pc)) +
    sign(dot(cross(ad, nor), pd))<3.0)
    ?
    min(min(min(
    dot2(ba*clamp(dot(ba, pa)/dot2(ba), 0.0, 1.0)-pa),
    dot2(cb*clamp(dot(cb, pb)/dot2(cb), 0.0, 1.0)-pb)),
    dot2(dc*clamp(dot(dc, pc)/dot2(dc), 0.0, 1.0)-pc)),
    dot2(ad*clamp(dot(ad, pd)/dot2(ad), 0.0, 1.0)-pd))
    :
    dot(nor, pa)*dot(nor, pa)/dot2(nor));
}

bool rayMarch(vec3 ro, vec3 rd, in LightVolume l) {
    float t = 0.0;
    for (int i = 0; i < 256; i++) {
        vec3 p = ro + t * rd;
        float d;

        switch (l.itemType){
            case ITEM_TYPE_SPHERE:{
                d = sdSphere(p, l.radiusSize, l.position);
                break;
            }
            case ITEM_TYPE_PLANE:{
                vec3 reference = abs(l.minNormal.y) > 0.999 ? vec3(1.0, 0.0, 0.0) : vec3(0.0, 1.0, 0.0);
                vec3 right = normalize(cross(l.minNormal, reference));
                vec3 up = normalize(cross(right, l.minNormal));

                vec3 halfRight = right * (l.radiusSize * 0.5);
                vec3 halfUp = up * (l.radiusSize * 0.5);

                vec3 a = l.position - halfRight - halfUp;
                vec3 b = l.position + halfRight - halfUp;
                vec3 c = l.position + halfRight + halfUp;
                vec3 dl = l.position - halfRight + halfUp;

                d = udQuad(p, a, b, c, dl);
                break;
            }
            default :{
                return false;
            }
        }
        if (d < 0.001) return true;
        if (t > 100.0) break;
        t += d;
    }
    return false;
}

void main(){
    if (globalData.lightVolumeCount == 0){
        discard;
    }
    vec3 dir = createRay(texCoords, globalData.invProj, globalData.invView);
    for (uint i = 0; i < globalData.lightVolumeCount; i++){
        LightVolume l = lightVolumeBuffer.items[i];
        if (rayMarch(globalData.cameraWorldPosition.xyz, dir, l)){
            finalColor = vec4(l.color, 1);
            return;
        }
    }
    discard;
}