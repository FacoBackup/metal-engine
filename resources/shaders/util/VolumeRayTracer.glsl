float sdfCuboid(vec3 p, vec3 b) {
    vec3 d = abs(p) - b;
    return length(max(d, 0.0)) + min(max(d.x, max(d.y, d.z)), 0.0);
}

float rayMarch(vec3 ro, vec3 rd, in LightVolume volume, out vec3 hitPos) {
    float t = 0.0;
    const float maxDistance = 100.0;
    const float surfaceThreshold = 0.001;

    for (int i = 0; i < 256; i++) {
        hitPos = ro + rd * t;
        vec3 localPos = hitPos - volume.position;
        float dist = sdfCuboid(localPos, volume.minNormal * 0.5);

        if (dist < surfaceThreshold) {
            return t;
        }

        if (t > maxDistance) {
            break;
        }

        t += dist;
    }

    return -1.0;// No hit
}

void traceVolumes(inout bool anyHit, inout vec3 finalColor, vec3 worldPosition, vec3 rayDirection){
    vec3 rayOrigin = globalData.cameraWorldPosition;
    float sceneDepth = length(worldPosition.rgb - rayOrigin);

    float closestDepth = 1e20;
    for (int i = 0; i < globalData.lightVolumeCount; i++) {
        LightVolume volume = lightVolumeBuffer.items[i];
        if (volume.itemType == ITEM_TYPE_VOLUME){
            vec3 hitPos;
            float depth = rayMarch(rayOrigin, rayDirection, volume, hitPos);

            if (depth > 0.0 && depth < closestDepth) {
                closestDepth = depth;
                if (depth < sceneDepth) {
                    finalColor = volume.color * exp(-volume.radiusSize * depth);
                    anyHit = true;
                }
            }
        }
    }
}