#include "../GlobalDataBuffer.glsl"

layout (push_constant) uniform Push {
    mat4 model;
    vec4 color;
    float fov;
    float zNear;
    float zFar;
    float aspectRatio;
} push;

const vec3[8] pyramidVertices = vec3[8](
    vec3(0, 0, 0), vec3(-1, -1, 1),
    vec3(0, 0, 0), vec3( 1, -1, 1),
    vec3(0, 0, 0), vec3( 1,  1, 1),
    vec3(0, 0, 0), vec3(-1,  1, 1)
);

const int[16] wireframeIndices = int[16](
    0, 1, 0, 2, 0, 3, 0, 4, // lines from apex to base
    1, 2, 2, 3, 3, 4, 4, 1  // base edges
);

void main() {
    float h = tan(radians(push.fov) * 0.5);
    float w = h * push.aspectRatio;
    float distance = 1.0; // visual length of the cone

    vec3 pos = vec3(0.0);
    
    // We'll use 16 vertices to draw 8 lines
    int lineIndex = gl_VertexIndex;
    
    if (lineIndex < 8) {
        // Lines from origin to base corners
        if (lineIndex % 2 == 0) {
            pos = vec3(0.0);
        } else {
            int corner = lineIndex / 2;
            if (corner == 0) pos = vec3(-w, -h, distance);
            if (corner == 1) pos = vec3( w, -h, distance);
            if (corner == 2) pos = vec3( w,  h, distance);
            if (corner == 3) pos = vec3(-w,  h, distance);
        }
    } else {
        // Base lines
        int baseLineIndex = lineIndex - 8;
        int corner = baseLineIndex / 2;
        int c1 = corner;
        int c2 = (corner + 1) % 4;
        int targetCorner = (baseLineIndex % 2 == 1) ? c2 : c1;
        
        if (targetCorner == 0) pos = vec3(-w, -h, distance);
        if (targetCorner == 1) pos = vec3( w, -h, distance);
        if (targetCorner == 2) pos = vec3( w,  h, distance);
        if (targetCorner == 3) pos = vec3(-w,  h, distance);
    }

    vec4 worldPos = push.model * vec4(pos, 1.0);
    gl_Position = globalData.projView * worldPos;
}
