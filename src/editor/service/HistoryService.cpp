#include "HistoryService.h"
#include <algorithm>
#include "common/Reflection.h"
#include "ApplicationEventContext.h"
#include "../dto/FieldModificationEvent.h"

namespace Metal {
    void HistoryService::startTransaction(const std::string &name) {
        if (currentTransaction) {
            endTransaction();
        }
        currentTransaction = std::make_unique<Transaction>();
        currentTransaction->name = name;
    }

    void HistoryService::endTransaction() {
        if (currentTransaction) {
            if (!currentTransaction->changes.empty()) {
                // Pick a representative key and instance for the whole transaction
                std::string key = currentTransaction->name;
                Reflection *instance = nullptr;
                std::string propertyName;

                for (const auto &change: currentTransaction->changes) {
                    if (std::holds_alternative<PropertyChange>(change)) {
                        const auto &pc = std::get<PropertyChange>(change);
                        instance = pc.field->instance;
                        propertyName = pc.field->name;
                        // If we find a property change, we use its instance's title as key if it's better than the generic transaction name
                        if (instance) key = instance->getTitle();
                        break;
                    }
                }

                if (instance) {
                    ApplicationEventContext::dispatch(instance->getClassName(),
                                                      std::make_shared<FieldModificationPayload>(
                                                          *instance->getFieldByName(propertyName)));
                }

                undoStack.push_back(std::move(*currentTransaction));
                redoStack.clear();
            }
            currentTransaction = nullptr;
        }
    }

    bool HistoryService::isTransactionActive() const {
        return currentTransaction != nullptr;
    }

    static PropertyValue getValueFromMember(FieldMetadata *field) {
        if (field->type == FLOAT) {
            return *static_cast<float *>(field->pointer);
        } else if (field->type == INT) {
            return *static_cast<int *>(field->pointer);
        } else if (field->type == BOOLEAN) {
            return *static_cast<bool *>(field->pointer);
        } else if (field->type == STRING || field->type == RESOURCE) {
            return *static_cast<std::string *>(field->pointer);
        } else if (field->type == VECTOR2) {
            return *static_cast<glm::vec2 *>(field->pointer);
        } else if (field->type == VECTOR3 || field->type == COLOR) {
            return *static_cast<glm::vec3 *>(field->pointer);
        } else if (field->type == VECTOR4) {
            return *static_cast<glm::vec4 *>(field->pointer);
        } else if (field->type == QUAT) {
            return *static_cast<glm::quat *>(field->pointer);
        }
        return 0;
    }

    void HistoryService::recordChange(FieldMetadata *field, const PropertyValue &oldValue) {
        if (!field) return;

        PropertyValue newValue = getValueFromMember(field);
        if (oldValue == newValue) return;

        std::shared_ptr<FieldMetadata> fieldPtr;
        if (field->instance) {
            fieldPtr = field->instance->getFieldByPointer(field->pointer);
        }

        if (!fieldPtr) return; // Cannot track if we can't get a shared_ptr to it

        PropertyChange change{fieldPtr, oldValue, newValue};

        if (currentTransaction) {
            auto it = std::find_if(currentTransaction->changes.begin(), currentTransaction->changes.end(),
                                   [&](const Change &c) {
                                       if (std::holds_alternative<PropertyChange>(c)) {
                                           const auto &pc = std::get<PropertyChange>(c);
                                           return pc.field == fieldPtr;
                                       }
                                       return false;
                                   });
            if (it != currentTransaction->changes.end()) {
                std::get<PropertyChange>(*it).newValue = newValue;
            } else {
                currentTransaction->changes.push_back(change);
            }
        } else {
            Transaction trans;
            trans.name = "Change " + field->name;
            trans.changes.push_back(change);
            undoStack.push_back(std::move(trans));
            redoStack.clear();
        }

        if (field->instance) {
            ApplicationEventContext::dispatch(field->instance->getClassName(), std::make_shared<FieldModificationPayload>(*field));
        }
    }

    void HistoryService::recordAction(const std::function<void()> &undoAction,
                                      const std::function<void()> &redoAction) {
        GenericChange change{undoAction, redoAction};

        if (currentTransaction) {
            currentTransaction->changes.push_back(change);
        } else {
            Transaction trans;
            trans.name = "Action";
            trans.changes.push_back(change);
            undoStack.push_back(std::move(trans));
            redoStack.clear();
        }
    }

    static void applyChange(FieldMetadata *field, const PropertyValue &value) {
        if (field->type == FLOAT) {
            *static_cast<float *>(field->pointer) = std::get<float>(value);
        } else if (field->type == INT) {
            *static_cast<int *>(field->pointer) = std::get<int>(value);
        } else if (field->type == BOOLEAN) {
            *static_cast<bool *>(field->pointer) = std::get<bool>(value);
        } else if (field->type == STRING || field->type == RESOURCE) {
            *static_cast<std::string *>(field->pointer) = std::get<std::string>(value);
        } else if (field->type == VECTOR2) {
            *static_cast<glm::vec2 *>(field->pointer) = std::get<glm::vec2>(value);
        } else if (field->type == VECTOR3 || field->type == COLOR) {
            *static_cast<glm::vec3 *>(field->pointer) = std::get<glm::vec3>(value);
        } else if (field->type == VECTOR4) {
            *static_cast<glm::vec4 *>(field->pointer) = std::get<glm::vec4>(value);
        } else if (field->type == QUAT) {
            *static_cast<glm::quat *>(field->pointer) = std::get<glm::quat>(value);
        }
    }

    void HistoryService::undo() {
        if (undoStack.empty()) {
            if (notificationService)
                notificationService->pushMessage("Nothing to undo.", NotificationSeverities::WARNING);
            return;
        }

        Transaction trans = std::move(undoStack.back());
        undoStack.pop_back();

        for (auto it = trans.changes.rbegin(); it != trans.changes.rend(); ++it) {
            if (std::holds_alternative<PropertyChange>(*it)) {
                const auto &change = std::get<PropertyChange>(*it);
                if (change.field && change.field->instance) {
                    applyChange(change.field.get(), change.oldValue);
                    ApplicationEventContext::dispatch(change.field->instance->getClassName(),
                                                      std::make_shared<FieldModificationPayload>(*change.field));
                }
            } else if (std::holds_alternative<GenericChange>(*it)) {
                const auto &change = std::get<GenericChange>(*it);
                if (change.undoAction) {
                    change.undoAction();
                }
            }
        }

        redoStack.push_back(std::move(trans));
    }

    void HistoryService::redo() {
        if (redoStack.empty()) {
            if (notificationService)
                notificationService->pushMessage("Nothing to redo.", NotificationSeverities::WARNING);
            return;
        }

        Transaction trans = std::move(redoStack.back());
        redoStack.pop_back();

        for (const auto &c: trans.changes) {
            if (std::holds_alternative<PropertyChange>(c)) {
                const auto &change = std::get<PropertyChange>(c);
                if (change.field && change.field->instance) {
                    applyChange(change.field.get(), change.newValue);
                    ApplicationEventContext::dispatch(change.field->instance->getClassName(),
                                                      std::make_shared<FieldModificationPayload>(*change.field));
                }
            } else if (std::holds_alternative<GenericChange>(c)) {
                const auto &change = std::get<GenericChange>(c);
                if (change.redoAction) {
                    change.redoAction();
                }
            }
        }

        undoStack.push_back(std::move(trans));
    }
}
