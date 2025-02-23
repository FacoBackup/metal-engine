#ifndef ENGINEID_H
#define ENGINEID_H

#define SURFACE_CACHE_RES 2000

#define MAX_RT_TRIANGLES 10000000
#define MAX_RT_BLAS 1000000
#define MAX_RT_TLAS 10000

typedef unsigned int EntityID;
#define MAX_LIGHT_VOLUMES 300
#define EMPTY_ENTITY 0
#define FILE_METADATA ".mjson"
#define FILE_SCENE "-scene"
#define FILE_MESH "-mesh"
#define FILE_TEXTURE "-texture"
#define METAL_FILE_EXTENSION ".metalasset"
#define FILE_MATERIAL "-material"
#define FILE_SVO "-svo"
#define FILE_NAME_SEPARATOR "##"

#define FORMAT_FILE_MESH(baseName, lod) (baseName + FILE_NAME_SEPARATOR + lod.suffix + FILE_MESH + METAL_FILE_EXTENSION)
#define FORMAT_FILE_TEXTURE(baseName, lod) (baseName + FILE_NAME_SEPARATOR + lod.suffix + FILE_TEXTURE + METAL_FILE_EXTENSION)
#define FORMAT_FILE_MATERIAL(baseName) (baseName + FILE_NAME_SEPARATOR + FILE_MATERIAL + METAL_FILE_EXTENSION)
#define FORMAT_FILE_SCENE(baseName) (baseName + FILE_NAME_SEPARATOR + FILE_SCENE + METAL_FILE_EXTENSION)
#define FORMAT_FILE_METADATA(baseName) (baseName + FILE_METADATA)
#define FORMAT_FILE_SVO(baseName) (baseName + FILE_NAME_SEPARATOR + FILE_SVO + METAL_FILE_EXTENSION)

#define METRIC_START \
auto currentTime = Clock::now(); \
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch());\
const auto start = duration.count();

#define METRIC_END(M) \
currentTime = Clock::now();\
duration = std::chrono::duration_cast< std::chrono::milliseconds>(currentTime.time_since_epoch());\
std::cout << M << " " << duration.count() - start << "ms" << std::endl;

#endif //ENGINEID_H
