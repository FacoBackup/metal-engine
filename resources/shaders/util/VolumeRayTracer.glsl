float hash(vec3 p) {
    p = fract(p * 0.3183099 + vec3(0.1, 0.2, 0.3));
    p *= 17.0;
    return fract(p.x * p.y * p.z * (p.x + p.y + p.z));
}

float densityVariation(vec3 p, float baseDensity) {
    float noise = hash(p * 0.5);
    return baseDensity * (0.5 + 0.5 * noise);
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


vec4 integrateVolume(vec3 ro, vec3 rd, in LightVolume volume, float sceneDepth) {
    vec3 roLocal = ro - volume.position;

    float tEntry, tExit;
    bool intersects = intersectBox(roLocal, rd, volume.minNormal, tEntry, tExit);
    if (!intersects) {
        return vec4(0.0);
    }

    if (tEntry > sceneDepth){
        return vec4(0.0);
    }

    tEntry = max(tEntry, 0.0);

    int steps = 64;
    float dt = (tExit - tEntry) / float(steps);

    vec3 scattering = vec3(0.0);
    float viewTransmittance = 1.0;

    for (int i = 0; i < steps; i++) {
        float tCurrent = tEntry + dt * (float(i) + 0.5);
        vec3 pos = ro + rd * tCurrent;
        vec3 localPos = pos - volume.position;

        float sampleDensity = densityVariation(localPos, volume.radiusSize);
        vec3 inScattered = vec3(0.0);
        for (int li = 0; li < globalData.volumesOffset; li++) {
            LightVolume light = lightVolumeBuffer.items[li];

            vec3 L = normalize(light.position - pos);
            float lightDist = length(light.position - pos);

            int shadowSteps = 8;
            float dtShadow = lightDist / float(shadowSteps);
            float opticalDepth = 0.0;
            for (int j = 0; j < shadowSteps; j++) {
                float tShadow = float(j) * dtShadow;
                vec3 posShadow = pos + L * tShadow;
                vec3 localShadow = posShadow - volume.position;
                opticalDepth += densityVariation(localShadow, volume.radiusSize) * dtShadow;
            }
            float lightTransmittance = exp(-opticalDepth);
            inScattered += light.color * lightTransmittance;
        }
        scattering += viewTransmittance * sampleDensity * inScattered * dt;
        viewTransmittance *= exp(-sampleDensity * dt);
    }
    float volumeAlpha = 1.0 - viewTransmittance;
    return vec4(scattering * volume.color, volumeAlpha);
}

void traceVolumes(inout vec4 finalColor, vec3 worldPosition, vec3 rayDirection){
    vec3 rayOrigin = globalData.cameraWorldPosition;
    float sceneDepth = length(worldPosition.rgb - rayOrigin);

    for (uint i = globalData.volumesOffset; i < globalData.lightVolumeCount; i++) {
        LightVolume volume = lightVolumeBuffer.items[i];
        finalColor += integrateVolume(rayOrigin, rayDirection, volume, sceneDepth);
    }
}