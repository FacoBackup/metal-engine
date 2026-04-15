#include "../GlobalDataBuffer.glsl"
#include "TerrainPushConstant.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outUV;
layout(location = 3) out vec3 outLocalPosition;
layout(location = 4) out vec4 outCurrPos;
layout(location = 5) out vec4 outPrevPos;

#extension GL_EXT_nonuniform_qualifier : enable
layout(set = 0, binding = 2) uniform sampler2D textureArray[];

void main(){
    // Local UV within the patch (0 to 1)
    vec2 patchUV = inUV;
    
    // Global UV for the whole terrain
    // Region: x, z, size, lod. Normalized to [0, 1] across the whole world.
    // Assuming worldSize is same as root size in QuadTreeService.
    float worldSize = 10000.0f; // This should ideally be passed in globalData
    vec2 globalUV = (push.region.xz + patchUV * push.region.y) / worldSize + 0.5;

    float height = texture(textureArray[nonuniformEXT(push.heightMapIndex)], globalUV).r;
    
    vec3 displacedPosition = inPosition;
    displacedPosition.x = push.region.x + patchUV.x * push.region.y;
    displacedPosition.z = push.region.z + patchUV.y * push.region.y;
    displacedPosition.y = height * push.terrainHeight;

    outNormal = vec3(0, 1, 0); // Should sample normal map if available
    outUV = globalUV;
    vec4 worldPos = vec4(displacedPosition, 1.0);
    outPosition = worldPos.rgb;
    outLocalPosition = displacedPosition;
    outCurrPos = globalData.projView * worldPos;
    outPrevPos = outCurrPos; // Simplified for terrain
    gl_Position =  outCurrPos;
}
