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
            undoStack.push_back(std::move(*currentTransaction));
            redoStack.clear();
        }
        currentTransaction = nullptr;
    }

    bool HistoryService::isTransactionActive() const {
        return currentTransaction != nullptr;
    }

    void HistoryService::recordChange(Inspectable *instance, const std::string &propertyPath,
                                      const PropertyValue &oldValue, const PropertyValue &newValue) {
        if (oldValue == newValue) return;

        PropertyChange change{instance, propertyPath, oldValue, newValue};

        if (currentTransaction) {
            auto it = std::find_if(currentTransaction->changes.begin(), currentTransaction->changes.end(),
                                   [&](const Change &c) {
                                       if (std::holds_alternative<PropertyChange>(c)) {
                                           const auto &pc = std::get<PropertyChange>(c);
                                           return pc.instance == instance && pc.propertyPath == propertyPath;
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
            trans.name = "Change " + propertyPath;
            trans.changes.push_back(change);
            undoStack.push_back(std::move(trans));
            redoStack.clear();
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

    static void applyChange(Inspectable *inspectable, const std::string &propertyPath, const PropertyValue &value) {
        for (const auto &field: inspectable->getFields()) {
            if (field->path == propertyPath) {
                if (field->type == FLOAT) {
                    auto f = dynamic_cast<InspectedField<float> *>(field.get());
                    if (f) *f->field = std::get<float>(value);
                } else if (field->type == INT) {
                    auto f = dynamic_cast<InspectedField<int> *>(field.get());
                    if (f) *f->field = std::get<int>(value);
                } else if (field->type == BOOLEAN) {
                    auto f = dynamic_cast<InspectedField<bool> *>(field.get());
                    if (f) *f->field = std::get<bool>(value);
                } else if (field->type == STRING || field->type == RESOURCE) {
                    auto f = dynamic_cast<InspectedField<std::string> *>(field.get());
                    if (f) *f->field = std::get<std::string>(value);
                } else if (field->type == VECTOR2) {
                    auto f = dynamic_cast<InspectedField<glm::vec2> *>(field.get());
                    if (f) *f->field = std::get<glm::vec2>(value);
                } else if (field->type == VECTOR3 || field->type == COLOR) {
                    auto f = dynamic_cast<InspectedField<glm::vec3> *>(field.get());
                    if (f) *f->field = std::get<glm::vec3>(value);
                } else if (field->type == VECTOR4) {
                    auto f = dynamic_cast<InspectedField<glm::vec4> *>(field.get());
                    if (f) *f->field = std::get<glm::vec4>(value);
                } else if (field->type == QUAT) {
                    auto f = dynamic_cast<InspectedField<glm::quat> *>(field.get());
                    if (f) *f->field = std::get<glm::quat>(value);
                }

                inspectable->registerChange();
                inspectable->onUpdate(field.get());
                break;
            }
        }
    }

    void HistoryService::undo() {
        if (undoStack.empty()) {
            notificationService->pushMessage("Nothing to undo.", NotificationSeverities::WARNING);
            return;
        }

        Transaction trans = std::move(undoStack.back());
        undoStack.pop_back();


        for (auto it = trans.changes.rbegin(); it != trans.changes.rend(); ++it) {
            if (std::holds_alternative<PropertyChange>(*it)) {
                const auto &change = std::get<PropertyChange>(*it);
                if (change.instance) {
                    applyChange(change.instance, change.propertyPath, change.oldValue);
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
            notificationService->pushMessage("Nothing to redo.", NotificationSeverities::WARNING);
            return;
        }

        Transaction trans = std::move(redoStack.back());
        redoStack.pop_back();

        for (const auto &c: trans.changes) {
            if (std::holds_alternative<PropertyChange>(c)) {
                const auto &change = std::get<PropertyChange>(c);
                if (change.instance) {
                    applyChange(change.instance, change.propertyPath, change.newValue);
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
