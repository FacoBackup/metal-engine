#include "LevelService.h"
#include "../repository/WorldRepository.h"
#include "common/FileExtensions.h"

namespace Metal {
    void LevelService::load(const std::string &absolutePath) {
        if (worldRepository) {
            worldRepository->clear();
            worldRepository->load(absolutePath);
        }
    }

    bool LevelService::isCompatible(const std::string &absolutePath) {
        return absolutePath.ends_with(FileExtensions::level->extension);
    }
}
