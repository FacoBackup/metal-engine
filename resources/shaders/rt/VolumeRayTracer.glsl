#include "../VolumesBuffer.glsl"
#include "../LightsBuffer.glsl"

vec3 RandomUnitVector();

vec2 sampleDisk() {
    float u = random();
    float v = random();
    float r = sqrt(u);
    float theta = 2.0 * 3.14159265359 * v;
    return vec2(r * cos(theta), r * sin(theta));
}

void computeOrthonormalBasis(in vec3 n, out vec3 tangent, out vec3 bitangent) {
    tangent = normalize(cross(n, abs(n.x) < 0.99 ? vec3(1, 0, 0) : vec3(0, 1, 0)));
    bitangent = cross(n, tangent);
}

float hash(vec3 p) {
    p = fract(p * 0.3183099 + vec3(0.1, 0.2, 0.3));
    p *= 17.0;
    return fract(p.x * p.y * p.z * (p.x + p.y + p.z));
}

// Henyey–Greenstein phase function
float phaseHenyeyGreenstein(vec3 wo, vec3 wi, float g) {
    float cosTheta = dot(wo, wi);
    float denom = 1.0 + g*g - 2.0 * g * cosTheta;
    return (1.0 - g*g) / (4.0 * 3.14159265359 * pow(denom, 1.5));
}
vec4 integrateVolume(vec3 ro, vec3 rd, in VolumeInstance volume, float sceneDepth) {
    vec3 roLocal = ro - volume.position;

    float tEntry, tExit;
    bool intersects = intersectBox(roLocal, rd, volume.size, tEntry, tExit);
    if (!intersects) {
        return vec4(0.0);
    }

    if (tEntry > sceneDepth){
        return vec4(0.0);
    }

    tEntry = max(tEntry, 0.0);

    int steps = int(volume.color.a);
    float dt = (tExit - tEntry) / float(steps);

    vec3 scattering = vec3(0.0);
    float viewTransmittance = 1.0;

    for (int i = 0; i < steps; i++) {
        float tCurrent = tEntry + dt * (float(i) + 0.5);
        vec3 pos = ro + rd * tCurrent;
        vec3 localPos = pos - volume.position;

        // **Scene Intersection Check**
        HitData hit = trace(ro, rd);
        if (hit.didHit && hit.closestT < tCurrent) {
            break;  // Stop marching if a surface is hit
        }

        float scatteringCoefficient = volume.density * volume.scatteringAlbedo;
        float absorptionCoefficient  = volume.density * (1.0 - volume.scatteringAlbedo);
        float extinctionCoefficient  = scatteringCoefficient + absorptionCoefficient;

        vec3 inScattered = vec3(0.0);
        for (int li = 0; li < globalData.lightCount; li++) {
            LightInstance light = lightsBuffer.items[li];
            vec3 samplePos;

            if (light.itemType == ITEM_TYPE_SPHERE) {
                vec3 randDir = RandomUnitVector();
                samplePos = light.position + randDir * light.scale.x;
            } else if (light.itemType == ITEM_TYPE_PLANE) {
                vec3 normal = normalize(light.planeNormal);
                vec3 tangent, bitangent;
                computeOrthonormalBasis(normal, tangent, bitangent);
                vec2 diskSample = sampleDisk();
                samplePos = light.position + (tangent * diskSample.x + bitangent * diskSample.y) * light.scale.x * light.scale.z;
            }

            vec3 L = normalize(samplePos - pos);
            float lightDist = length(samplePos - pos);

            // **Scene Shadow Check**
            HitData shadowHit = trace(pos + L * 0.001, L);
            bool isShadowed = shadowHit.didHit && shadowHit.closestT < lightDist;

            if (!isShadowed) {
                float dtShadow = lightDist / float(globalData.volumeShadowSteps);
                float opticalDepth = 0.0;
                for (int j = 0; j < globalData.volumeShadowSteps; j++) {
                    float tShadow = float(j) * dtShadow;
                    vec3 posShadow = pos + L * tShadow;
                    vec3 localShadow = posShadow - volume.position;
                    opticalDepth += extinctionCoefficient * dtShadow;
                }
                float lightTransmittance = exp(-opticalDepth);
                float g = volume.phaseFunctionAsymmetry;
                float phase = phaseHenyeyGreenstein(rd, L, g);
                inScattered += light.color.rgb * light.color.a * lightTransmittance * phase;
            }
        }
        scattering += viewTransmittance * scatteringCoefficient * inScattered * dt;
        viewTransmittance *= exp(-max(extinctionCoefficient * dt, 0.0001));

        if (viewTransmittance < 0.01) {
            break; // Early exit if transmission is too low
        }
    }
    float volumeAlpha = 1.0 - viewTransmittance;
    return vec4(scattering * volume.color.rgb, volumeAlpha);
}

void traceVolumes(inout vec4 finalColor, vec3 worldPosition, vec3 rayDirection){
    vec3 rayOrigin = globalData.cameraWorldPosition;
    float sceneDepth = length(worldPosition.rgb - rayOrigin);

    for (uint i = 0; i < globalData.volumeCount; i++) {
        VolumeInstance volume = volumesBuffer.items[i];
        finalColor += integrateVolume(rayOrigin, rayDirection, volume, sceneDepth);
    }
}