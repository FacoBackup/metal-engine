#include "../util/HWRayTracingUtil.glsl"
#include "../util/RayTracerUtil.glsl"
#include "../util/LightVisibility.glsl"
#include "../MeshDataSampling.glsl"

// Principled PBR Path tracer. Except where otherwise noted:

// Copyright © 2019 Markus Moenig Distributed under The MIT License.
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// Based on an collaboration with pixotronics.com

// Implementation of http://disney-animation.s3.amazonaws.com/library/s2012_pbs_disney_brdf_notes_v2.pdf

// Disney brdf's taken from here:: https://github.com/wdas/brdf/blob/master/src/brdfs/disney.brdf
float schlickWeight(float cosTheta) {
    float m = clamp(1. - cosTheta, 0., 1.);
    return (m * m) * (m * m) * m;
}

float GTR1(float NdotH, float a) {
    if (a >= 1.) return 1. / PI;
    float a2 = a * a;
    float t = 1. + (a2 - 1.) * NdotH * NdotH;
    return (a2 - 1.) / (PI * log(a2) * t);
}

float GTR2(float NdotH, float a) {
    float a2 = a * a;
    float t = 1. + (a2 - 1.) * NdotH * NdotH;
    return a2 / (PI * t * t);
}

float GTR2_aniso(float NdotH, float HdotX, float HdotY, float ax, float ay) {
    return 1. / (PI * ax * ay * pow2(pow2(HdotX / ax) + pow2(HdotY / ay) + NdotH * NdotH));
}

float smithG_GGX(float NdotV, float alphaG) {
    float a = alphaG * alphaG;
    float b = NdotV * NdotV;
    return 1. / (abs(NdotV) + max(sqrt(a + b - a * b), EPSILON));
}

float smithG_GGX_aniso(float NdotV, float VdotX, float VdotY, float ax, float ay) {
    return 1. / (NdotV + sqrt(pow2(VdotX * ax) + pow2(VdotY * ay) + pow2(NdotV)));
}

float pdfLambertianReflection(const in vec3 wi, const in vec3 wo, const in vec3 normal) {
    return sameHemiSphere(wo, wi, normal) ? abs(dot(normal, wi)) / PI : 0.;
}

float pdfMicrofacet(const in vec3 wi, const in vec3 wo, const in SurfaceInteraction interaction, const in MaterialInfo material) {
    if (!sameHemiSphere(wo, wi, interaction.normal)) return 0.;
    vec3 wh = normalize(wo + wi);

    float NdotH = dot(interaction.normal, wh);
    float alpha2 = material.roughness * material.roughness;
    alpha2 *= alpha2;

    float cos2Theta = NdotH * NdotH;
    float denom = cos2Theta * (alpha2 - 1.) + 1.;
    if (denom == 0.) return 0.;
    float pdfDistribution = alpha2 * NdotH / (PI * denom * denom);
    return pdfDistribution / (4. * dot(wo, wh));
}

float pdfMicrofacetAniso(const in vec3 wi, const in vec3 wo, const in vec3 X, const in vec3 Y, const in SurfaceInteraction interaction, const in MaterialInfo material) {
    if (!sameHemiSphere(wo, wi, interaction.normal)) return 0.;
    vec3 wh = normalize(wo + wi);

    float aspect = sqrt(1. - material.anisotropic * .9);
    float alphax = max(.001, pow2(material.roughness) / aspect);
    float alphay = max(.001, pow2(material.roughness) * aspect);

    float alphax2 = alphax * alphax;
    float alphay2 = alphay * alphay;

    float hDotX = dot(wh, X);
    float hDotY = dot(wh, Y);
    float NdotH = dot(interaction.normal, wh);

    float denom = hDotX * hDotX / alphax2 + hDotY * hDotY / alphay2 + NdotH * NdotH;
    if (denom == 0.) return 0.;
    float pdfDistribution = NdotH / (PI * alphax * alphay * denom * denom);
    return pdfDistribution / (4. * dot(wo, wh));
}

float pdfClearCoat(const in vec3 wi, const in vec3 wo, const in SurfaceInteraction interaction, const in MaterialInfo material) {
    if (!sameHemiSphere(wo, wi, interaction.normal)) return 0.;

    vec3 wh = wi + wo;
    wh = normalize(wh);

    float NdotH = abs(dot(wh, interaction.normal));
    float Dr = GTR1(NdotH, mix(.1, .001, material.clearcoatGloss));
    return Dr * NdotH / (4. * dot(wo, wh));
}

vec3 disneyDiffuse(const in float NdotL, const in float NdotV, const in float LdotH, const in MaterialInfo material) {

    float FL = schlickWeight(NdotL), FV = schlickWeight(NdotV);

    float Fd90 = 0.5 + 2. * LdotH * LdotH * material.roughness;
    float Fd = mix(1.0, Fd90, FL) * mix(1.0, Fd90, FV);

    return (1. / PI) * Fd * material.baseColor;
}

vec3 disneySubsurface(const in float NdotL, const in float NdotV, const in float LdotH, const in MaterialInfo material) {

    float FL = schlickWeight(NdotL), FV = schlickWeight(NdotV);
    float Fss90 = LdotH * LdotH * material.roughness;
    float Fss = mix(1.0, Fss90, FL) * mix(1.0, Fss90, FV);
    float ss = 1.25 * (Fss * (1. / (NdotL + NdotV) - .5) + .5);

    return (1. / PI) * ss * material.baseColor;
}

vec3 disneyMicrofacetIsotropic(float NdotL, float NdotV, float NdotH, float LdotH, const in MaterialInfo material) {

    float Cdlum = .3 * material.baseColor.r + .6 * material.baseColor.g + .1 * material.baseColor.b;// luminance approx.

    vec3 Ctint = Cdlum > 0. ? material.baseColor / Cdlum : vec3(1.);// normalize lum. to isolate hue+sat
    vec3 Cspec0 = mix(material.specular * .08 * mix(vec3(1.), Ctint, material.specularTint), material.baseColor, material.metallic);

    float a = max(.001, pow2(material.roughness));
    float Ds = GTR2(NdotH, a);
    float FH = schlickWeight(LdotH);
    vec3 Fs = mix(Cspec0, vec3(1), FH);
    float Gs;
    Gs = smithG_GGX(NdotL, a);
    Gs *= smithG_GGX(NdotV, a);

    return Gs * Fs * Ds;
}

vec3 disneyMicrofacetAnisotropic(float NdotL, float NdotV, float NdotH, float LdotH,
const in vec3 L, const in vec3 V,
const in vec3 H, const in vec3 X, const in vec3 Y,
const in MaterialInfo material) {

    float Cdlum = .3 * material.baseColor.r + .6 * material.baseColor.g + .1 * material.baseColor.b;

    vec3 Ctint = Cdlum > 0. ? material.baseColor / Cdlum : vec3(1.);
    vec3 Cspec0 = mix(material.specular * .08 * mix(vec3(1.), Ctint, material.specularTint), material.baseColor, material.metallic);

    float aspect = sqrt(1. - material.anisotropic * .9);
    float ax = max(.001, pow2(material.roughness) / aspect);
    float ay = max(.001, pow2(material.roughness) * aspect);
    float Ds = GTR2_aniso(NdotH, dot(H, X), dot(H, Y), ax, ay);
    float FH = schlickWeight(LdotH);
    vec3 Fs = mix(Cspec0, vec3(1), FH);
    float Gs;
    Gs = smithG_GGX_aniso(NdotL, dot(L, X), dot(L, Y), ax, ay);
    Gs *= smithG_GGX_aniso(NdotV, dot(V, X), dot(V, Y), ax, ay);

    return Gs * Fs * Ds;
}

float disneyClearCoat(float NdotL, float NdotV, float NdotH, float LdotH, const in MaterialInfo material) {
    float gloss = mix(.1, .001, material.clearcoatGloss);
    float Dr = GTR1(abs(NdotH), gloss);
    float FH = schlickWeight(LdotH);
    float Fr = mix(.04, 1.0, FH);
    float Gr = smithG_GGX(NdotL, .25) * smithG_GGX(NdotV, .25);
    return clearCoatBoost * material.clearcoat * Fr * Gr * Dr;
}

vec3 disneySheen(float LdotH, const in MaterialInfo material) {
    float FH = schlickWeight(LdotH);
    float Cdlum = .3 * material.baseColor.r + .6 * material.baseColor.g + .1 * material.baseColor.b;

    vec3 Ctint = Cdlum > 0. ? material.baseColor / Cdlum : vec3(1.);
    vec3 Csheen = mix(vec3(1.), Ctint, material.sheenTint);
    vec3 Fsheen = FH * material.sheen * Csheen;
    return FH * material.sheen * Csheen;
}

vec3 perpendicular(const vec3 v) {
    if (abs(v.x) < abs(v.y)) {
        return normalize(vec3(0.0, -v.z, v.y));
    }
    return normalize(vec3(-v.z, 0.0, v.x));
}


vec3 lightSample(const in Light light, const in SurfaceInteraction interaction, out vec3 wi, out float lightPdf, out vec3 emission, out float dist) {
    PrimitiveData primitive = primitiveBuffer.items[light.meshIndex];
    TriangleData tri = getTriangleData(primitive, light.triangleIndex);

    // Sample triangle
    vec2 u = vec2(random(), random());
    float r1 = sqrt(u.x);
    float r2 = u.y;
    vec3 lightPos = (1.0 - r1) * tri.v0 + (r1 * (1.0 - r2)) * tri.v1 + (r1 * r2) * tri.v2;

    vec3 edge1 = tri.v1 - tri.v0;
    vec3 edge2 = tri.v2 - tri.v0;
    vec3 lightNormal = normalize(cross(edge1, edge2));
    float area = 0.5 * length(cross(edge1, edge2));

    wi = lightPos - interaction.point;
    float distSq = dot(wi, wi);
    dist = sqrt(distSq);
    wi /= dist;

    float cosThetaLight = dot(lightNormal, -wi);
    if (cosThetaLight > 0.0) {
        lightPdf = distSq / (cosThetaLight * area);
        lightPdf /= float(globalData.lightsCount);

        emission = light.color * pushConstants.pathTracingEmissiveFactor;
        float visibility = visibilityTest(dist, interaction.point, wi);
        return emission * visibility;
    }

    lightPdf = 0.0;
    emission = vec3(0.0);
    dist = 0.0;
    return vec3(0.0);
}

float lightPdf(const in Light light, const in SurfaceInteraction interaction, vec3 wi) {
    PrimitiveData primitive = primitiveBuffer.items[light.meshIndex];
    TriangleData tri = getTriangleData(primitive, light.triangleIndex);

    vec3 edge1 = tri.v1 - tri.v0;
    vec3 edge2 = tri.v2 - tri.v0;
    vec3 lightNormal = normalize(cross(edge1, edge2));
    float area = 0.5 * length(cross(edge1, edge2));

    // Intersection test with the triangle
    float denom = dot(lightNormal, wi);
    if (abs(denom) < EPSILON) return 0.0;

    float t = dot(tri.v0 - interaction.point, lightNormal) / denom;
    if (t < 0.0) return 0.0;

    vec3 p = interaction.point + t * wi;

    // Barycentric coordinates for point-in-triangle test
    vec3 v2_p = p - tri.v0;
    float d00 = dot(edge1, edge1);
    float d01 = dot(edge1, edge2);
    float d11 = dot(edge2, edge2);
    float d20 = dot(v2_p, edge1);
    float d21 = dot(v2_p, edge2);
    float invDenom = 1.0 / (d00 * d11 - d01 * d01);
    float v = (d11 * d20 - d01 * d21) * invDenom;
    float w = (d00 * d21 - d01 * d20) * invDenom;
    float u = 1.0 - v - w;

    if (u >= 0.0 && v >= 0.0 && w >= 0.0) {
        float cosThetaLight = abs(dot(lightNormal, -wi));
        float distSq = t * t;
        return distSq / (cosThetaLight * area * float(globalData.lightsCount));
    }

    return 0.0;
}

vec3 bsdfEvaluate(const in vec3 wi, const in vec3 wo, const in vec3 X, const in vec3 Y, const in SurfaceInteraction interaction, const in MaterialInfo material) {
    bool reflected = sameHemiSphere(wo, wi, interaction.normal);

    if (!reflected)
    return vec3(0.);

    float NdotL = dot(interaction.normal, wo);
    float NdotV = dot(interaction.normal, wi);

    if (NdotL < 0. || NdotV < 0.) return vec3(0.);

    vec3 H = normalize(wo + wi);
    float NdotH = dot(interaction.normal, H);
    float LdotH = dot(wo, H);

    vec3 diffuse = disneyDiffuse(NdotL, NdotV, LdotH, material);
    vec3 subSurface = disneySubsurface(NdotL, NdotV, LdotH, material);
    vec3 glossy = disneyMicrofacetAnisotropic(NdotL, NdotV, NdotH, LdotH, wi, wo, H, X, Y, material);
    float clearCoat = disneyClearCoat(NdotL, NdotV, NdotH, LdotH, material);
    vec3 sheen = disneySheen(LdotH, material);

    return (mix(diffuse, subSurface, material.subsurface) + sheen) * (1. - material.metallic) + glossy + clearCoat;
}



void disneyDiffuseSample(out vec3 wi, const in vec3 wo, out float pdf, const in vec2 u, const in vec3 normal, const in MaterialInfo material) {
    //    COSINE SAMPLE
    vec3 wiLocal = cosineSampleHemisphere(u);
    vec3 tangent = vec3(0.), binormal = vec3(0.);
    createBasis(normal, tangent, binormal);
    wi = wiLocal.x * tangent + wiLocal.y * binormal + wiLocal.z * normal;
    if (dot(wo, normal) < 0.) wi.z *= -1.;
    //    COSINE SAMPLE

}

vec3 disneySubSurfaceSample(out vec3 wi, const in vec3 wo, out float pdf, const in vec2 u, const in vec3 normal, const in MaterialInfo material) {
    //    COSINE SAMPLE
    vec3 wiLocal = cosineSampleHemisphere(u);
    vec3 tangent = vec3(0.), binormal = vec3(0.);
    createBasis(normal, tangent, binormal);
    wi = wiLocal.x * tangent + wiLocal.y * binormal + wiLocal.z * normal;
    if (dot(wo, normal) < 0.) wi.z *= -1.;
    //    COSINE SAMPLE

    vec3 H = normalize(wo + wi);
    float NdotH = dot(normal, H);

    pdf = pdfLambertianReflection(wi, wo, normal);
    return vec3(0.);//disneySubsurface(NdotL, NdotV, NdotH, material) * material.subsurface;
}

vec3 disneySheenSample(out vec3 wi, const in vec3 wo, out float pdf, const in vec2 u, const in vec3 normal, const in MaterialInfo material) {
    //    COSINE SAMPLE
    vec3 wiLocal = cosineSampleHemisphere(u);
    vec3 tangent = vec3(0.), binormal = vec3(0.);
    createBasis(normal, tangent, binormal);
    wi = wiLocal.x * tangent + wiLocal.y * binormal + wiLocal.z * normal;
    if (dot(wo, normal) < 0.) wi.z *= -1.;
    //    COSINE SAMPLE

    vec3 H = normalize(wo + wi);
    float LdotH = dot(wo, H);

    pdf = pdfLambertianReflection(wi, wo, normal);
    return disneySheen(LdotH, material);
}

vec3 disneyMicrofacetSample(out vec3 wi, const in vec3 wo, out float pdf, const in vec2 u, const in SurfaceInteraction interaction, const in MaterialInfo material) {
    float cosTheta = 0., phi = (2. * PI) * u[1];
    float alpha = material.roughness * material.roughness;
    float tanTheta2 = alpha * alpha * u[0] / (1.0 - u[0]);
    cosTheta = 1. / sqrt(1. + tanTheta2);

    float sinTheta = sqrt(max(EPSILON, 1. - cosTheta * cosTheta));
    vec3 whLocal = sphericalDirection(sinTheta, cosTheta, sin(phi), cos(phi));

    vec3 tangent = vec3(0.), binormal = vec3(0.);
    createBasis(interaction.normal, tangent, binormal);

    vec3 wh = whLocal.x * tangent + whLocal.y * binormal + whLocal.z * interaction.normal;

    if (!sameHemiSphere(wo, wh, interaction.normal)) {
        wh *= -1.;
    }

    wi = reflect(-wo, wh);

    float NdotL = dot(interaction.normal, wo);
    float NdotV = dot(interaction.normal, wi);

    if (NdotL < 0. || NdotV < 0.) {
        pdf = 0.;// If not set to 0 here, create's artifacts. WHY EVEN IF SET OUTSIDE??
        return vec3(0.);
    }

    vec3 H = normalize(wo + wi);
    float NdotH = dot(interaction.normal, H);
    float LdotH = dot(wo, H);

    pdf = pdfMicrofacet(wi, wo, interaction, material);
    return disneyMicrofacetIsotropic(NdotL, NdotV, NdotH, LdotH, material);
}

void disneyMicrofacetAnisoSample(out vec3 wi, const in vec3 wo, const in vec3 X, const in vec3 Y, const in vec2 u, const in SurfaceInteraction interaction, const in MaterialInfo material) {
    float cosTheta = 0., phi = 0.;

    float aspect = sqrt(1. - material.anisotropic * .9);
    float alphax = max(.001, pow2(material.roughness) / aspect);
    float alphay = max(.001, pow2(material.roughness) * aspect);

    phi = atan(alphay / alphax * tan(2. * PI * u[1] + 0.5 * PI));

    if (u[1] > 0.5) phi += PI;
    float sinPhi = sin(phi), cosPhi = cos(phi);
    float alphax2 = alphax * alphax, alphay2 = alphay * alphay;
    float alpha2 = 1. / (cosPhi * cosPhi / alphax2 + sinPhi * sinPhi / alphay2);
    float tanTheta2 = alpha2 * u[0] / (1. - u[0]);
    cosTheta = 1. / sqrt(1. + tanTheta2);

    float sinTheta = sqrt(max(0., 1. - cosTheta * cosTheta));
    vec3 whLocal = sphericalDirection(sinTheta, cosTheta, sin(phi), cos(phi));

    vec3 wh = whLocal.x * X + whLocal.y * Y + whLocal.z * interaction.normal;

    if (!sameHemiSphere(wo, wh, interaction.normal)) {
        wh *= -1.;
    }

    wi = reflect(-wo, wh);
}

void disneyClearCoatSample(out vec3 wi, const in vec3 wo, const in vec2 u, const in SurfaceInteraction interaction, const in MaterialInfo material) {
    float gloss = mix(0.1, 0.001, material.clearcoatGloss);
    float alpha2 = gloss * gloss;
    float cosTheta = sqrt(max(EPSILON, (1. - pow(alpha2, 1. - u[0])) / (1. - alpha2)));
    float sinTheta = sqrt(max(EPSILON, 1. - cosTheta * cosTheta));
    float phi = TWO_PI * u[1];

    vec3 whLocal = sphericalDirection(sinTheta, cosTheta, sin(phi), cos(phi));

    vec3 tangent = vec3(0.), binormal = vec3(0.);
    createBasis(interaction.normal, tangent, binormal);

    vec3 wh = whLocal.x * tangent + whLocal.y * binormal + whLocal.z * interaction.normal;

    if (!sameHemiSphere(wo, wh, interaction.normal)) {
        wh *= -1.;
    }

    wi = reflect(-wo, wh);
}

float bsdfPdf(const in vec3 wi, const in vec3 wo, const in vec3 X, const in vec3 Y, const in SurfaceInteraction interaction, const in MaterialInfo material) {
    float pdfDiffuse = pdfLambertianReflection(wi, wo, interaction.normal);
    float pdfMicrofacet = pdfMicrofacetAniso(wi, wo, X, Y, interaction, material);
    float pdfClearCoat = pdfClearCoat(wi, wo, interaction, material);;
    return (pdfDiffuse + pdfMicrofacet + pdfClearCoat) / 3.;
}

float light_pdf(const in Light light, const in SurfaceInteraction interaction, vec3 wi) {
    return lightPdf(light, interaction, wi);
}

vec3 bsdfSample(out vec3 wi, const in vec3 wo, const in vec3 X, const in vec3 Y, out float pdf, const in SurfaceInteraction interaction, const in MaterialInfo material) {

    vec3 f = vec3(0.);
    pdf = 0.0;
    wi = vec3(0.);

    vec2 u = vec2(random(), random());
    float rndBSDF = random();

    if (rndBSDF <= 0.3333) {
        disneyDiffuseSample(wi, wo, pdf, u, interaction.normal, material);
    } else if (rndBSDF > 0.3333 && rndBSDF < 0.6666) {
        disneyMicrofacetAnisoSample(wi, wo, X, Y, u, interaction, material);
    } else {
        disneyClearCoatSample(wi, wo, u, interaction, material);
    }

    f = bsdfEvaluate(wi, wo, X, Y, interaction, material);
    pdf = bsdfPdf(wi, wo, X, Y, interaction, material);

    if (pdf < EPSILON) {
        return vec3(0.);
    }
    return f;
}

float powerHeuristic(float nf, float fPdf, float ng, float gPdf) {
    float f = nf * fPdf;
    float g = ng * gPdf;
    return (f * f) / (f * f + g * g);
}

vec3 calculateDirectLight(const in Light light, const in SurfaceInteraction interaction, const in MaterialInfo material, out vec3 wi, out vec3 f, out float scatteringPdf) {
    vec3 wo = -interaction.incomingRayDir;
    vec3 Ld = vec3(0.);
    float lightPdfValue = 0.;
    scatteringPdf = 0.;
    vec3 emission = vec3(0.);

    float lightDist = 0.0;
    vec3 Li = lightSample(light, interaction, wi, lightPdfValue, emission, lightDist);

    if (lightPdfValue > EPSILON && dot(Li, Li) > 0.0) {
        vec3 fDirect = bsdfEvaluate(wi, wo, interaction.tangent, interaction.binormal, interaction, material) * abs(dot(wi, interaction.normal));

        if (dot(fDirect, fDirect) > 0.0) {
            Ld += Li * fDirect / lightPdfValue;
        }
    }

    // BSDF Sample
    f = bsdfSample(wi, wo, interaction.tangent, interaction.binormal, scatteringPdf, interaction, material);
    f *= abs(dot(wi, interaction.normal));

    return Ld;
}