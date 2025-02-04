#ifndef L_V
#define L_V
struct LightVolume {
    vec4 color;
    vec3 position;
    vec3 dataA;
    vec3 dataB;// Density for volume
    uint itemType;
};
#endif

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

float densityVariation(vec3 p, float baseDensity) {
//    float noise = hash(p * 0.5);
    return baseDensity; //* (0.5 + 0.5 * noise);
}

bool intersectBox(vec3 ro, vec3 rd, vec3 dimensions, out float tEntry, out float tExit) {
    vec3 boxMin = -dimensions * 0.5;
    vec3 boxMax =  dimensions * 0.5;
    vec3 invRd = 1.0 / rd;

    vec3 t0s = (boxMin - ro) * invRd;
    vec3 t1s = (boxMax - ro) * invRd;
    vec3 tsmaller = min(t0s, t1s);
    vec3 tbigger  = max(t0s, t1s);

    tEntry = max(max(tsmaller.x, tsmaller.y), tsmaller.z);
    tExit  = min(min(tbigger.x, tbigger.y), tbigger.z);

    return (tExit >= max(tEntry, 0.0));
}

// Henyey–Greenstein phase function
float phaseHenyeyGreenstein(vec3 wo, vec3 wi, float g) {
    float cosTheta = dot(wo, wi);
    float denom = 1.0 + g*g - 2.0 * g * cosTheta;
    return (1.0 - g*g) / (4.0 * 3.14159265359 * pow(denom, 1.5));
}

vec4 integrateVolume(vec3 ro, vec3 rd, in LightVolume volume, float sceneDepth) {
    vec3 roLocal = ro - volume.position;

    float tEntry, tExit;
    bool intersects = intersectBox(roLocal, rd, volume.dataA, tEntry, tExit);
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

        float density = densityVariation(localPos, volume.dataB.x);

        float scatteringAlbedo = volume.dataB.y;// fraction of extinction that is scattering
        float scatteringCoefficient = density * scatteringAlbedo;
        float absorptionCoefficient  = density * (1.0 - scatteringAlbedo);
        float extinctionCoefficient  = scatteringCoefficient + absorptionCoefficient;

        vec3 inScattered = vec3(0.0);
        for (int li = 0; li < globalData.volumesOffset; li++) {
            LightVolume light = lightVolumeBuffer.items[li];
            vec3 samplePos;

            if (light.itemType == ITEM_TYPE_SPHERE) {
                vec3 randDir = RandomUnitVector();
                samplePos = light.position + randDir * light.dataB.x;
            } else if (light.itemType == ITEM_TYPE_PLANE) {
                vec3 normal = normalize(light.dataA);
                vec3 tangent, bitangent;
                computeOrthonormalBasis(normal, tangent, bitangent);
                vec2 diskSample = sampleDisk();
                samplePos = light.position + (tangent * diskSample.x + bitangent * diskSample.y) * light.dataB.x;
            }

            vec3 L = normalize(samplePos - pos);
            float lightDist = length(samplePos - pos);

            float dtShadow = lightDist / float(globalData.volumeShadowSteps);
            float opticalDepth = 0.0;
            for (int j = 0; j < globalData.volumeShadowSteps; j++) {
                float tShadow = float(j) * dtShadow;
                vec3 posShadow = pos + L * tShadow;
                vec3 localShadow = posShadow - volume.position;
                opticalDepth += densityVariation(localShadow, volume.dataB.x) * dtShadow;
            }
            float lightTransmittance = exp(-opticalDepth);
            float g = volume.dataB.z;// phase function asymmetry parameter; 0.0 for isotropic
            float phase = phaseHenyeyGreenstein(rd, L, g);
            inScattered += light.color.rgb * light.color.a * lightTransmittance * phase;
        }
        scattering += viewTransmittance * scatteringCoefficient * inScattered * dt;
        viewTransmittance *= exp(-extinctionCoefficient * dt);
    }
    float volumeAlpha = 1.0 - viewTransmittance;
    return vec4(scattering * volume.color.rgb, volumeAlpha);
}

void traceVolumes(inout vec4 finalColor, vec3 worldPosition, vec3 rayDirection){
    vec3 rayOrigin = globalData.cameraWorldPosition;
    float sceneDepth = length(worldPosition.rgb - rayOrigin);

    for (uint i = globalData.volumesOffset; i < globalData.lightVolumeCount; i++) {
        LightVolume volume = lightVolumeBuffer.items[i];
        finalColor += integrateVolume(rayOrigin, rayDirection, volume, sceneDepth);
    }
}