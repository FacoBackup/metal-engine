#include <gtest/gtest.h>
#include "../src/ApplicationContext.h"
#include "../src/editor/service/HistoryService.h"
#include "../src/editor/service/EventService.h"
#include "../src/editor/service/NotificationService.h"
#include "../src/common/Inspectable.h"
#include <vector>

#include "../src/editor/dto/FieldModificationEvent.h"

using namespace Metal;

class MockHistoryObserver : public IContextMember {
public:
    std::vector<Event> receivedEvents;

    void startObserving(const std::string &key) {
        eventListener([this](const Event &event) {
            receivedEvents.push_back(event);
        }, key);
    }

    void stopObserving() {
        // Automatically handled by IContextMember destructor
    }
};

class MockInspectableForEvents : public Inspectable {
public:
    int field = 0;
    int other = 0;

    void registerFields() override {
        registerInt(field, "Group", "Field");
        registerInt(other, "Group", "Other");
    }

    const char *getIcon() override { return ""; }
    const char *getTitle() override { return "Mock"; }
};

class EventServiceTest : public ::testing::Test {
protected:
    std::shared_ptr<ApplicationContext> context;
    HistoryService *historyService;
    MockHistoryObserver observer;
    MockInspectableForEvents mock;

    void SetUp() override {
        context = std::make_shared<ApplicationContext>(true);
        context->registerSingleton(std::make_shared<NotificationService>());
        context->registerSingleton(std::make_shared<HistoryService>());

        historyService = &context->getSingleton<HistoryService>();

        context->injectDependencies(historyService);
        context->injectDependencies(&observer);

        observer.startObserving("Mock");
        observer.startObserving("Undo");
        observer.startObserving("Redo");
        observer.startObserving("Action");
        mock.getFields();
    }

    void TearDown() override {
    }
};

TEST_F(EventServiceTest, TriggersChangeRegistered) {
    mock.field = 10;
    historyService->recordChange(mock.getFieldByPointer(&mock.field).get(), 0);

    ASSERT_EQ(observer.receivedEvents.size(), 1);
    EXPECT_EQ(observer.receivedEvents[0].key, "Mock");
    auto payload = std::dynamic_pointer_cast<FieldModificationPayload>(observer.receivedEvents[0].payload);
    ASSERT_NE(payload, nullptr);
    EXPECT_EQ(payload->member.instance, &mock);
}

TEST_F(EventServiceTest, TriggersUndo) {
    mock.field = 10;
    historyService->recordChange(mock.getFieldByPointer(&mock.field).get(), 0);
    observer.receivedEvents.clear();

    historyService->undo();

    ASSERT_EQ(observer.receivedEvents.size(), 1);
    EXPECT_EQ(observer.receivedEvents[0].key, "Undo");
}

TEST_F(EventServiceTest, TriggersRedo) {
    mock.field = 10;
    historyService->recordChange(mock.getFieldByPointer(&mock.field).get(), 0);
    historyService->undo();
    observer.receivedEvents.clear();

    historyService->redo();

    ASSERT_EQ(observer.receivedEvents.size(), 1);
    EXPECT_EQ(observer.receivedEvents[0].key, "Redo");
}

TEST_F(EventServiceTest, UnsubscribeWorks) {
    observer.stopObserving();

    mock.field = 10;
    historyService->recordChange(mock.getFieldByPointer(&mock.field).get(), 0);

    EXPECT_EQ(observer.receivedEvents.size(), 0);
}

TEST_F(EventServiceTest, ActionTriggersChangeRegistered) {
    historyService->recordAction([]() {
                                 }, []() {
                                 });

    ASSERT_EQ(observer.receivedEvents.size(), 1);
    EXPECT_EQ(observer.receivedEvents[0].key, "Action");
}

TEST_F(EventServiceTest, DispatchModelWorks) {
    MockHistoryObserver keyObserver;
    context->injectDependencies(&keyObserver);
    keyObserver.startObserving("MyKey");

    // This should trigger keyObserver
    EventService::dispatch("MyKey");

    // This should NOT trigger keyObserver (wrong key)
    EventService::dispatch("OtherKey");

    ASSERT_EQ(keyObserver.receivedEvents.size(), 1);
    EXPECT_EQ(keyObserver.receivedEvents[0].key, "MyKey");

    keyObserver.stopObserving();
}

TEST_F(EventServiceTest, TransactionTriggersEvent) {
    historyService->startTransaction("MyTransaction");
    auto field = mock.getFieldByPointer(&mock.field).get();
    mock.field = 10;
    historyService->recordChange(field, 0);
    mock.field = 20;
    historyService->recordChange(field, 10);

    // No event yet
    ASSERT_EQ(observer.receivedEvents.size(), 0);

    historyService->endTransaction();

    // Should trigger one event for the transaction
    ASSERT_EQ(observer.receivedEvents.size(), 1);
    EXPECT_EQ(observer.receivedEvents[0].key, "Mock");
}
