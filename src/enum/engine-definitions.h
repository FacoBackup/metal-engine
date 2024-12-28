#ifndef ENGINEID_H
#define ENGINEID_H


typedef unsigned long long EntityID;
#define MAX_LIGHTS 300
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
#define FORMAT_FILE_SVO(baseName, lod) (baseName + FILE_NAME_SEPARATOR + lod.suffix + FILE_SVO + METAL_FILE_EXTENSION)

#endif //ENGINEID_H
