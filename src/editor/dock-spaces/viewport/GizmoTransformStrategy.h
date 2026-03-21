#ifndef GIZMOTRANSFORMSTRATEGY_H
#define GIZMOTRANSFORMSTRATEGY_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <array>

namespace Metal {
    struct TransformComponent;
    struct EditorRepository;
    struct WorldRepository;
    class HistoryService;

    class GizmoTransformStrategy {
        glm::mat4x4 cacheProjection{};
        glm::mat4x4 cacheMatrixMat4{};
        glm::vec3 auxTranslation{};
        glm::vec3 auxScale{};
        glm::quat auxRot{};
        bool wasUsing = false;

        float cacheMatrix[16]{};
        float *viewMatrixCache = nullptr;
        float *projectionMatrixCache = nullptr;
        std::array<float, 3> translationSnap;

        HistoryService *historyService;
        EditorRepository *editorRepository;
        WorldRepository *worldRepository;

    public:
        GizmoTransformStrategy(HistoryService *historyService, EditorRepository *editorRepository,
                               WorldRepository *worldRepository);

        ~GizmoTransformStrategy() = default;

        void updateCache(TransformComponent *selected);

        void recomposeMatrix();

        void decomposeMatrix(TransformComponent *selected);

        void updateUsingState(bool isUsing);

        float *getCacheMatrix() { return cacheMatrix; }
        float *getViewMatrixCache() { return viewMatrixCache; }
        float *getProjectionMatrixCache() { return projectionMatrixCache; }

        float *getSnapValues();
    };
}

#endif //GIZMOTRANSFORMSTRATEGY_H
