layout(location = 0) in vec2 texCoords;
#define A  0.15
#define B  0.50
#define C  0.10
#define D  0.20
#define E  0.02
#define F  0.30
#define W  11.2

layout(push_constant) uniform Push {
    float   distortionIntensity;
    float   chromaticAberrationIntensity;
    bool    distortionEnabled;
    bool    chromaticAberrationEnabled;
    bool    vignetteEnabled;
    float   vignetteStrength;
} postProcessing;

layout(set = 0, binding = 0) uniform sampler2D sceneColor;

layout(location = 0) out vec4 finalColor;


vec3 chromaticAberration(vec2 uv) {
    float amount = postProcessing.chromaticAberrationIntensity * .001;
    vec3 col;
    col.r = texture(sceneColor, vec2(uv.x + amount, uv.y)).r;
    col.g = texture(sceneColor, uv).g;
    col.b = texture(sceneColor, vec2(uv.x - amount, uv.y)).b;
    return col;
}

vec2 lensDistortion(vec2 uv, float k) {
    vec2 t = uv - .5;
    float r2 = t.x * t.x + t.y * t.y;
    float f = 1. + r2 * (.1 - k * sqrt(r2));

    vec2 nUv = f * t + .5;
    return nUv;
}
// Uchimura 2017, "HDR theory and practice"
// Math: https://www.desmos.com/calculator/gslcdxvipg
// Source: https://www.slideshare.net/nikuque/hdr-theory-and-practicce-jp
vec3 uchimura(vec3 x, float P, float a, float m, float l, float c, float b) {
    float l0 = ((P - m) * l) / a;
    float L0 = m - m / a;
    float L1 = m + (1.0 - m) / a;
    float S0 = m + l0;
    float S1 = m + a * l0;
    float C2 = (a * P) / (P - S1);
    float CP = -C2 / P;

    vec3 w0 = vec3(1.0 - smoothstep(0.0, m, x));
    vec3 w2 = vec3(step(m + l0, x));
    vec3 w1 = vec3(1.0 - w0 - w2);

    vec3 T = vec3(m * pow(x / m, vec3(c)) + b);
    vec3 S = vec3(P - (P - S1) * exp(CP * (x - S0)));
    vec3 L = vec3(m + a * (x - m));

    return T * w0 + L * w1 + S * w2;
}

vec3 uchimura(vec3 x) {
    const float P = 1.0;  // max display brightness
    const float a = 1.0;  // contrast
    const float m = 0.22; // linear section start
    const float l = 0.4;  // linear section length
    const float c = 1.33; // black
    const float b = 0.0;  // pedestal

    return uchimura(x, P, a, m, l, c, b);
}

vec3 neutral(vec3 color) {
    const float startCompression = 0.8 - 0.04;
    const float desaturation = 0.15;

    float x = min(color.r, min(color.g, color.b));
    float offset = x < 0.08 ? x - 6.25 * x * x : 0.04;
    color -= offset;

    float peak = max(color.r, max(color.g, color.b));
    if (peak < startCompression) return color;

    const float d = 1.0 - startCompression;
    float newPeak = 1.0 - d * d / (peak + d - startCompression);
    color *= newPeak / peak;

    float g = 1.0 - 1.0 / (desaturation * (peak - newPeak) + 1.0);
    return mix(color, vec3(newPeak), g);
}

vec3 aces(vec3 x) {
    // Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return (x * (a * x + b)) / (x * (c * x + d) + e);
}

void main() {

    vec2 texCoords = postProcessing.distortionEnabled ? lensDistortion(texCoords, postProcessing.distortionIntensity * .5) : texCoords;
    vec3 color = postProcessing.chromaticAberrationEnabled ? chromaticAberration(texCoords) : texture(sceneColor, texCoords).rgb;

    finalColor = vec4(neutral(color), 1.);

    if (postProcessing.vignetteEnabled) {
        vec2 uv = texCoords;
        uv *= 1.0 - uv.yx;
        float vig = pow(uv.x * uv.y * 15., postProcessing.vignetteStrength);
        finalColor.rgb *= vig;
    }
}