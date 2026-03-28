#ifndef HISTORYSERVICE_H
#define HISTORYSERVICE_H

#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <variant>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <functional>

#include "NotificationService.h"
#include "common/IService.h"

namespace Metal {
    class Reflection;
    struct FieldMetadata;

    using PropertyValue = std::variant<std::string, int, float, bool, glm::vec2, glm::vec3, glm::vec4, glm::quat>;

    struct PropertyChange {
        std::shared_ptr<FieldMetadata> field;
        PropertyValue oldValue;
        PropertyValue newValue;
    };

    struct GenericChange {
        std::function<void()> undoAction;
        std::function<void()> redoAction;
    };

    using Change = std::variant<PropertyChange, GenericChange>;

    struct Transaction {
        std::string name;
        std::vector<Change> changes;
    };

    class HistoryService final : public IService {
        std::vector<Transaction> undoStack;
        std::vector<Transaction> redoStack;
        std::unique_ptr<Transaction> currentTransaction = nullptr;
        NotificationService *notificationService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"notificationService", &notificationService}
            };
        }

        void startTransaction(const std::string &name);

        void endTransaction();

        bool isTransactionActive() const;

        void recordChange(FieldMetadata *field, const PropertyValue &oldValue);

        void recordAction(const std::function<void()> &undoAction, const std::function<void()> &redoAction);

        void undo();

        void redo();

        bool canUndo() const { return !undoStack.empty(); }
        bool canRedo() const { return !redoStack.empty(); }
    };
} // Metal

#endif // HISTORYSERVICE_H
