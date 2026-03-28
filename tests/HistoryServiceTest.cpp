#include <gtest/gtest.h>
#include "../src/editor/service/HistoryService.h"
#include "../src/common/Reflection.h"
#include "../src/common/IEventMember.h"
#include "../src/ApplicationEventContext.h"
#include <glm/glm.hpp>

using namespace Metal;

class MockInspectable : public Reflection {
public:
    int intField = 0;
    float floatField = 0.0f;
    std::string stringField = "";
    bool boolField = false;
    glm::vec3 vec3Field = {0, 0, 0};

    void registerFields() override {
        registerEditableField<INT>(&intField).setName("IntField").setGroup("Group");
        registerEditableField<FLOAT>(&floatField).setName("FloatField").setGroup("Group");
        registerEditableField<STRING>(&stringField).setName("StringField").setGroup("Group");
        registerEditableField<BOOLEAN>(&boolField).setName("BoolField").setGroup("Group");
        registerEditableField<VECTOR3>(&vec3Field).setName("Vec3Field").setGroup("Group");
    }
    const char* getIcon() const override { return ""; }
    const char* getTitle() const override { return "Mock"; }
    std::string getClassName() const override { return "MockInspectable"; }
};

class HistoryServiceTest : public ::testing::Test, public IEventMember {
protected:
    HistoryService historyService;
    MockInspectable mock;
    std::vector<std::string> eventLog;

    void SetUp() override {
        mock.getFields();
        eventLog.clear();
        eventListener([this](const Event &e) {
            eventLog.push_back(e.key);
        }, "MockInspectable");
    }
};

TEST_F(HistoryServiceTest, UndoRedoFiresEvents) {
    int oldValue = 0;
    int newValue = 42;
    auto field = mock.getFieldByPointer(&mock.intField);

    // Initial change
    mock.intField = newValue;
    historyService.recordChange(field.get(), oldValue);
    ASSERT_EQ(eventLog.size(), 1);
    EXPECT_EQ(eventLog[0], "MockInspectable");
    eventLog.clear();

    // Undo should fire event
    historyService.undo();
    EXPECT_EQ(mock.intField, oldValue);
    ASSERT_EQ(eventLog.size(), 1);
    EXPECT_EQ(eventLog[0], "MockInspectable");
    eventLog.clear();

    // Redo should fire event
    historyService.redo();
    EXPECT_EQ(mock.intField, newValue);
    ASSERT_EQ(eventLog.size(), 1);
    EXPECT_EQ(eventLog[0], "MockInspectable");
}

TEST_F(HistoryServiceTest, RecordSingleChange) {
    int oldValue = 0;
    int newValue = 42;

    auto field = mock.getFieldByPointer(&mock.intField);
    mock.intField = newValue;
    historyService.recordChange(field.get(), oldValue);

    EXPECT_TRUE(historyService.canUndo());
    EXPECT_FALSE(historyService.canRedo());
}

TEST_F(HistoryServiceTest, UndoRedoSingleChange) {
    int oldValue = 0;
    int newValue = 42;
    mock.intField = newValue; // Simulated change

    auto field = mock.getFieldByPointer(&mock.intField);
    historyService.recordChange(field.get(), oldValue);

    historyService.undo();
    EXPECT_EQ(mock.intField, oldValue);
    EXPECT_FALSE(historyService.canUndo());
    EXPECT_TRUE(historyService.canRedo());

    historyService.redo();
    EXPECT_EQ(mock.intField, newValue);
    EXPECT_TRUE(historyService.canUndo());
    EXPECT_FALSE(historyService.canRedo());
}

TEST_F(HistoryServiceTest, TransactionMultipleChanges) {
    historyService.startTransaction("MultiChange");

    mock.intField = 10;
    historyService.recordChange(mock.getFieldByPointer(&mock.intField).get(), 0);
    mock.floatField = 1.0f;
    historyService.recordChange(mock.getFieldByPointer(&mock.floatField).get(), 0.0f);
    historyService.endTransaction();

    EXPECT_TRUE(historyService.canUndo());

    mock.intField = 10;
    mock.floatField = 1.0f;

    historyService.undo();
    EXPECT_EQ(mock.intField, 0);
    EXPECT_EQ(mock.floatField, 0.0f);

    historyService.redo();
    EXPECT_EQ(mock.intField, 10);
    EXPECT_EQ(mock.floatField, 1.0f);
}

TEST_F(HistoryServiceTest, TransactionCollapseSameField) {
    historyService.startTransaction("Collapse");
    auto field = mock.getFieldByPointer(&mock.intField).get();
    mock.intField = 10;
    historyService.recordChange(field, 0);
    mock.intField = 20;
    historyService.recordChange(field, 10);
    historyService.endTransaction();

    mock.intField = 20;

    historyService.undo();
    // It should go back to the original value before the transaction started
    EXPECT_EQ(mock.intField, 0);

    historyService.redo();
    EXPECT_EQ(mock.intField, 20);
}

TEST_F(HistoryServiceTest, RedoStackClearedOnNewChange) {
    auto field = mock.getFieldByPointer(&mock.intField).get();
    mock.intField = 10;
    historyService.recordChange(field, 0);
    historyService.undo();
    EXPECT_TRUE(historyService.canRedo());

    mock.intField = 20;
    historyService.recordChange(field, 0);
    EXPECT_FALSE(historyService.canRedo());
}

TEST_F(HistoryServiceTest, Vec3Change) {
    glm::vec3 oldVal{0, 0, 0};
    glm::vec3 newVal{1, 2, 3};
    mock.vec3Field = newVal;

    historyService.recordChange(mock.getFieldByPointer(&mock.vec3Field).get(), oldVal);

    historyService.undo();
    EXPECT_EQ(mock.vec3Field, oldVal);

    historyService.redo();
    EXPECT_EQ(mock.vec3Field, newVal);
}

TEST_F(HistoryServiceTest, GenericAction) {
    int value = 0;

    auto undo = [&]() { value = 0; };
    auto redo = [&]() { value = 42; };

    historyService.recordAction(undo, redo);

    value = 42; // Simulated initial redo

    historyService.undo();
    EXPECT_EQ(value, 0);

    historyService.redo();
    EXPECT_EQ(value, 42);
}

TEST_F(HistoryServiceTest, InvalidFieldCrash) {
    auto field = mock.getFieldByPointer(&mock.intField);
    std::shared_ptr<FieldMetadata> fieldPtr = field;

    historyService.recordChange(fieldPtr.get(), 0);

    // Simulate destruction of the reflectionInstance
    // In our case, mock is on the stack, but we can clear the instance pointer in the field
    // or just let it point to a destroyed object.
    // To be safe in a test, we can just set instance to nullptr
    fieldPtr->instance = nullptr;

    // This should NOT crash
    EXPECT_NO_THROW(historyService.undo());
    EXPECT_NO_THROW(historyService.redo());
}

TEST_F(HistoryServiceTest, NullFieldInTransaction) {
    historyService.startTransaction("CrashTest");
    historyService.recordChange(nullptr, 0);
    historyService.endTransaction();

    EXPECT_NO_THROW(historyService.undo());
    EXPECT_NO_THROW(historyService.redo());
}
