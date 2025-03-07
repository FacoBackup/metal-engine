#define DITHER_MATRIX mat4(\
     0.0, 0.5, 0.125, 0.625, \
     0.75, 0.25, 0.875, 0.375, \
     0.1875, 0.6875, 0.0625, 0.5625, \
     0.9375, 0.4375, 0.8125, 0.312\
)

bool isDitherDiscard(float alpha){
    #ifdef IS_COMPUTE
    ivec2 coords = ivec2(mod(gl_GlobalInvocationID.xy, 4.0));
    #else
    ivec2 coords = ivec2(mod(gl_FragCoord.xy, 4.0));
    #endif
    float threshold = DITHER_MATRIX[coords.y][coords.x];
    return alpha <= threshold;
}