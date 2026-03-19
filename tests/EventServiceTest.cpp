#include <gtest/gtest.h>
#include "../src/ApplicationContext.h"
#include "../src/editor/service/HistoryService.h"
#include "../src/editor/service/EventService.h"
#include "../src/editor/service/NotificationService.h"
#include "../src/common/Inspectable.h"
#include <vector>

using namespace Metal;

class MockHistoryObserver : public IContextMember {
public:
    EventService* eventService = nullptr;
    std::vector<Event> receivedEvents;
    std::map<std::string, int> subscriptions;

    std::vector<Dependency> getDependencies() override {
        return {
            {"eventService", &eventService}
        };
    }

    void startObserving(const std::string& key) {
        if (eventService) {
            subscriptions[key] = eventService->subscribe(key, [this](const Event& event) {
                receivedEvents.push_back(event);
            });
        }
    }

    void stopObserving() {
        if (eventService) {
            for (auto const& [key, id] : subscriptions) {
                eventService->unsubscribe(id);
            }
            subscriptions.clear();
        }
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
    const char* getIcon() override { return ""; }
    const char* getTitle() override { return "Mock"; }
};

class EventServiceTest : public ::testing::Test {
protected:
    std::shared_ptr<ApplicationContext> context;
    HistoryService* historyService;
    EventService* eventService;
    MockHistoryObserver observer;
    MockInspectableForEvents mock;

    void SetUp() override {
        context = std::make_shared<ApplicationContext>(true);
        context->registerSingleton(std::make_shared<NotificationService>());
        context->registerSingleton(std::make_shared<EventService>());
        context->registerSingleton(std::make_shared<HistoryService>());
        
        historyService = &context->getSingleton<HistoryService>();
        eventService = &context->getSingleton<EventService>();
        
        context->injectDependencies(historyService);
        context->injectDependencies(&observer);
        
        observer.startObserving("Mock");
        observer.startObserving("Undo");
        observer.startObserving("Redo");
        observer.startObserving("Action");
        mock.getFields();
    }

    void TearDown() override {
        observer.stopObserving();
    }
};

TEST_F(EventServiceTest, TriggersChangeRegistered) {
    mock.field = 10;
    historyService->recordChange(mock.getFieldByPointer(&mock.field).get(), 0);
    
    ASSERT_EQ(observer.receivedEvents.size(), 1);
    EXPECT_EQ(observer.receivedEvents[0].key, "Mock");
    auto payload = std::dynamic_pointer_cast<InspectableEventPayload>(observer.receivedEvents[0].payload);
    ASSERT_NE(payload, nullptr);
    EXPECT_EQ(payload->instance, &mock);
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
    historyService->recordAction([](){}, [](){});
    
    ASSERT_EQ(observer.receivedEvents.size(), 1);
    EXPECT_EQ(observer.receivedEvents[0].key, "Action");
}

TEST_F(EventServiceTest, DispatchModelWorks) {
    MockHistoryObserver keyObserver;
    context->injectDependencies(&keyObserver);
    keyObserver.startObserving("MyKey");

    // This should trigger keyObserver
    eventService->dispatch("MyKey");
    
    // This should NOT trigger keyObserver (wrong key)
    eventService->dispatch("OtherKey");
    
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
