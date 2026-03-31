#ifndef RESERVOIR_GLSL
#define RESERVOIR_GLSL

struct Reservoir {
    vec3 radiance;      // Li
    vec3 sampleDir;     // Direction to sample
    float distance;     // Distance to sample (optional for ReSTIR GI)
    float W;            // Combined weight (W = w_sum / (M * target_pdf))
    float w_sum;        // Running sum of weights
    uint M;             // Number of samples processed
};

// Pack reservoir for storage (optimize for memory if needed)
struct PackedReservoir {
    vec4 radianceDist;   // xyz: radiance, w: distance
    vec4 dirW;           // xyz: sampleDir, w: W
    vec2 sumM;           // x: w_sum, y: M (float representation)
};

PackedReservoir packReservoir(Reservoir r) {
    PackedReservoir p;
    p.radianceDist = vec4(r.radiance, r.distance);
    p.dirW = vec4(r.sampleDir, r.W);
    p.sumM = vec2(r.w_sum, float(r.M));
    return p;
}

Reservoir unpackReservoir(PackedReservoir p) {
    Reservoir r;
    r.radiance = p.radianceDist.xyz;
    r.distance = p.radianceDist.w;
    r.sampleDir = p.dirW.xyz;
    r.W = p.dirW.w;
    r.w_sum = p.sumM.x;
    r.M = uint(p.sumM.y);
    return r;
}

bool updateReservoir(inout Reservoir r, vec3 radiance, vec3 sampleDir, float distance, float weight, float rand) {
    r.w_sum += weight;
    r.M++;
    if (rand * r.w_sum < weight) {
        r.radiance = radiance;
        r.sampleDir = sampleDir;
        r.distance = distance;
        return true;
    }
    return false;
}

// targetPdf is typically luminance of (radiance * BSDF)
void finalizeReservoir(inout Reservoir r, float targetPdf) {
    if (targetPdf > 0.0) {
        r.W = r.w_sum / (max(float(r.M), 1.0) * targetPdf);
    } else {
        r.W = 0.0;
    }
}

#endif
