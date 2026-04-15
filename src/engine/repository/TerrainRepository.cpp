#include "TerrainRepository.h"
#include "../../common/FileExtensions.h"

namespace Metal {
    void TerrainRepository::registerFields() {
        registerEditableField<RESOURCE>(&heightMap).setName("heightMap").setLabel("Terrain Height Map").setSupportedFileTypes({FileExtensions::png.get(), FileExtensions::jpg.get(), FileExtensions::tga.get()});
        registerEditableField<FLOAT>(&terrainSize).setName("terrainSize").setLabel("Terrain Size").setMin(100.0f).setMax(100000.0f).setIncrement(100.0f);
        registerEditableField<FLOAT>(&terrainHeight).setName("terrainHeight").setLabel("Terrain Height Scale").setMin(0.0f).setMax(10000.0f).setIncrement(10.0f);
        registerEditableField<INT>(&terrainMaxLOD).setName("terrainMaxLOD").setLabel("Terrain Max LOD").setMin(1).setMax(15).setIncrement(1);

        registerEditableField<RESOURCE>(&layer1).setName("layer1").setLabel("Layer 1 Material").setSupportedFileTypes({FileExtensions::material.get()});
        registerEditableField<RESOURCE>(&layer2).setName("layer2").setLabel("Layer 2 Material").setSupportedFileTypes({FileExtensions::material.get()});
        registerEditableField<RESOURCE>(&layer3).setName("layer3").setLabel("Layer 3 Material").setSupportedFileTypes({FileExtensions::material.get()});
        registerEditableField<RESOURCE>(&layer4).setName("layer4").setLabel("Layer 4 Material").setSupportedFileTypes({FileExtensions::material.get()});
    }
} // Metal
