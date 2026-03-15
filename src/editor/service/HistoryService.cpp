#include "HistoryService.h"
#include <algorithm>
#include "../../common/Inspectable.h"
#include "../../common/InspectedField.h"

namespace Metal {
    void HistoryService::startTransaction(const std::string &name) {
        if (currentTransaction) {
            endTransaction();
        }
        currentTransaction = std::make_unique<Transaction>();
        currentTransaction->name = name;
    }

    void HistoryService::endTransaction() {
        if (currentTransaction && !currentTransaction->changes.empty()) {
            // Pick a representative key and instance for the whole transaction
            std::string key = currentTransaction->name;
            Inspectable* instance = nullptr;
            std::string propertyPath;

            for (const auto& change : currentTransaction->changes) {
                if (std::holds_alternative<PropertyChange>(change)) {
                    const auto& pc = std::get<PropertyChange>(change);
                    instance = pc.field->instance;
                    propertyPath = pc.field->path;
                    // If we find a property change, we use its instance's title as key if it's better than the generic transaction name
                    if (instance) key = instance->getTitle();
                    break; 
                }
            }

            undoStack.push_back(std::move(*currentTransaction));
            redoStack.clear();
            
            HistoryEvent event;
            event.key = key;
            event.instance = instance;
            event.propertyPath = propertyPath;
            if (instance) {
                event.instanceType = typeid(*instance);
            }
            if (historyEventService) historyEventService->trigger(event);
        }
        currentTransaction = nullptr;
    }

    bool HistoryService::isTransactionActive() const {
        return currentTransaction != nullptr;
    }

    static PropertyValue getValueFromMember(InspectableMember *field) {
        if (field->type == FLOAT) {
            auto f = dynamic_cast<InspectedField<float> *>(field);
            if (f) return *f->field;
        } else if (field->type == INT) {
            auto f = dynamic_cast<InspectedField<int> *>(field);
            if (f) return *f->field;
        } else if (field->type == BOOLEAN) {
            auto f = dynamic_cast<InspectedField<bool> *>(field);
            if (f) return *f->field;
        } else if (field->type == STRING || field->type == RESOURCE) {
            auto f = dynamic_cast<InspectedField<std::string> *>(field);
            if (f) return *f->field;
        } else if (field->type == VECTOR2) {
            auto f = dynamic_cast<InspectedField<glm::vec2> *>(field);
            if (f) return *f->field;
        } else if (field->type == VECTOR3 || field->type == COLOR) {
            auto f = dynamic_cast<InspectedField<glm::vec3> *>(field);
            if (f) return *f->field;
        } else if (field->type == VECTOR4) {
            auto f = dynamic_cast<InspectedField<glm::vec4> *>(field);
            if (f) return *f->field;
        } else if (field->type == QUAT) {
            auto f = dynamic_cast<InspectedField<glm::quat> *>(field);
            if (f) return *f->field;
        }
        return 0;
    }

    void HistoryService::recordChange(InspectableMember *field, const PropertyValue &oldValue) {
        PropertyValue newValue = getValueFromMember(field);
        if (oldValue == newValue) return;

        PropertyChange change{field, oldValue, newValue};

        if (currentTransaction) {
            auto it = std::find_if(currentTransaction->changes.begin(), currentTransaction->changes.end(),
                                   [&](const Change &c) {
                                       if (std::holds_alternative<PropertyChange>(c)) {
                                           const auto &pc = std::get<PropertyChange>(c);
                                           return pc.field == field;
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
            trans.name = "Change " + field->path;
            trans.changes.push_back(change);
            undoStack.push_back(std::move(trans));
            redoStack.clear();

            HistoryEvent event;
            event.key = field->instance->getTitle();
            event.instance = field->instance;
            event.propertyPath = field->path;
            event.instanceType = typeid(*field->instance);
            if (historyEventService) historyEventService->trigger(event);
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
            
            HistoryEvent event;
            event.key = "Action";
            if (historyEventService) historyEventService->trigger(event);
        }
    }

    static void applyChange(InspectableMember *field, const PropertyValue &value) {
        if (field->type == FLOAT) {
            auto f = dynamic_cast<InspectedField<float> *>(field);
            if (f) *f->field = std::get<float>(value);
        } else if (field->type == INT) {
            auto f = dynamic_cast<InspectedField<int> *>(field);
            if (f) *f->field = std::get<int>(value);
        } else if (field->type == BOOLEAN) {
            auto f = dynamic_cast<InspectedField<bool> *>(field);
            if (f) *f->field = std::get<bool>(value);
        } else if (field->type == STRING || field->type == RESOURCE) {
            auto f = dynamic_cast<InspectedField<std::string> *>(field);
            if (f) *f->field = std::get<std::string>(value);
        } else if (field->type == VECTOR2) {
            auto f = dynamic_cast<InspectedField<glm::vec2> *>(field);
            if (f) *f->field = std::get<glm::vec2>(value);
        } else if (field->type == VECTOR3 || field->type == COLOR) {
            auto f = dynamic_cast<InspectedField<glm::vec3> *>(field);
            if (f) *f->field = std::get<glm::vec3>(value);
        } else if (field->type == VECTOR4) {
            auto f = dynamic_cast<InspectedField<glm::vec4> *>(field);
            if (f) *f->field = std::get<glm::vec4>(value);
        } else if (field->type == QUAT) {
            auto f = dynamic_cast<InspectedField<glm::quat> *>(field);
            if (f) *f->field = std::get<glm::quat>(value);
        }
    }

    void HistoryService::undo() {
        if (undoStack.empty()) {
            if (notificationService) notificationService->pushMessage("Nothing to undo.", NotificationSeverities::WARNING);
            return;
        }

        Transaction trans = std::move(undoStack.back());
        undoStack.pop_back();


        for (auto it = trans.changes.rbegin(); it != trans.changes.rend(); ++it) {
            if (std::holds_alternative<PropertyChange>(*it)) {
                const auto &change = std::get<PropertyChange>(*it);
                if (change.field) {
                    applyChange(change.field, change.oldValue);
                }
            } else if (std::holds_alternative<GenericChange>(*it)) {
                const auto &change = std::get<GenericChange>(*it);
                if (change.undoAction) {
                    change.undoAction();
                }
            }
        }

        redoStack.push_back(std::move(trans));
        
        HistoryEvent event;
        event.key = "Undo";
        if (historyEventService) historyEventService->trigger(event);
    }

    void HistoryService::redo() {
        if (redoStack.empty()) {
            if (notificationService) notificationService->pushMessage("Nothing to redo.", NotificationSeverities::WARNING);
            return;
        }

        Transaction trans = std::move(redoStack.back());
        redoStack.pop_back();

        for (const auto &c: trans.changes) {
            if (std::holds_alternative<PropertyChange>(c)) {
                const auto &change = std::get<PropertyChange>(c);
                if (change.field) {
                    applyChange(change.field, change.newValue);
                }
            } else if (std::holds_alternative<GenericChange>(c)) {
                const auto &change = std::get<GenericChange>(c);
                if (change.redoAction) {
                    change.redoAction();
                }
            }
        }

        undoStack.push_back(std::move(trans));
        
        HistoryEvent event;
        event.key = "Redo";
        if (historyEventService) historyEventService->trigger(event);
    }
}
