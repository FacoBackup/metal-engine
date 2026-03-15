#include <gtest/gtest.h>
#include "../src/ApplicationContext.h"
#include "../src/editor/service/HistoryService.h"
#include "../src/editor/service/HistoryEventService.h"
#include "../src/editor/service/NotificationService.h"
#include "../src/common/Inspectable.h"
#include <vector>

using namespace Metal;

class MockHistoryObserver : public IContextMember {
public:
    HistoryEventService* historyEventService = nullptr;
    std::vector<HistoryEvent> receivedEvents;
    int subscriptionId = -1;
    int genericSubscriptionId = -1;

    std::vector<Dependency> getDependencies() override {
        return {
            {"historyEventService", &historyEventService}
        };
    }

    void startObserving() {
        if (historyEventService) {
            subscriptionId = historyEventService->subscribe([this](const HistoryEvent& event) {
                receivedEvents.push_back(event);
            });
        }
    }

    template<typename T>
    void startObservingType(const std::string& path = "") {
        if (historyEventService) {
            subscriptionId = historyEventService->subscribe<T>([this](const HistoryEvent& event) {
                receivedEvents.push_back(event);
            }, path);
        }
    }

    void startObservingGeneric() {
        if (historyEventService) {
            genericSubscriptionId = historyEventService->subscribeGeneric([this](const HistoryEvent& event) {
                receivedEvents.push_back(event);
            });
        }
    }

    void stopObserving() {
        if (historyEventService) {
            if (subscriptionId != -1) {
                historyEventService->unsubscribe(subscriptionId);
                subscriptionId = -1;
            }
            if (genericSubscriptionId != -1) {
                historyEventService->unsubscribe(genericSubscriptionId);
                genericSubscriptionId = -1;
            }
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

class HistoryEventServiceTest : public ::testing::Test {
protected:
    std::shared_ptr<ApplicationContext> context;
    HistoryService* historyService;
    HistoryEventService* historyEventService;
    MockHistoryObserver observer;
    MockInspectableForEvents mock;

    void SetUp() override {
        context = std::make_shared<ApplicationContext>(true);
        context->registerSingleton(std::make_shared<NotificationService>());
        context->registerSingleton(std::make_shared<HistoryEventService>());
        context->registerSingleton(std::make_shared<HistoryService>());
        
        historyService = &context->getSingleton<HistoryService>();
        historyEventService = &context->getSingleton<HistoryEventService>();
        
        context->injectDependencies(historyService);
        context->injectDependencies(&observer);
        
        observer.startObserving();
        mock.getFields();
    }

    void TearDown() override {
        observer.stopObserving();
    }
};

TEST_F(HistoryEventServiceTest, TriggersChangeRegistered) {
    mock.field = 10;
    historyService->recordChange(mock.getFieldByPointer(&mock.field).get(), 0);
    
    ASSERT_EQ(observer.receivedEvents.size(), 1);
    EXPECT_EQ(observer.receivedEvents[0].key, "Mock");
    EXPECT_EQ(observer.receivedEvents[0].instance, &mock);
    EXPECT_EQ(observer.receivedEvents[0].instanceType, typeid(mock));
}

TEST_F(HistoryEventServiceTest, TriggersUndo) {
    mock.field = 10;
    historyService->recordChange(mock.getFieldByPointer(&mock.field).get(), 0);
    observer.receivedEvents.clear();
    
    historyService->undo();
    
    ASSERT_EQ(observer.receivedEvents.size(), 1);
    EXPECT_EQ(observer.receivedEvents[0].key, "Undo");
}

TEST_F(HistoryEventServiceTest, TriggersRedo) {
    mock.field = 10;
    historyService->recordChange(mock.getFieldByPointer(&mock.field).get(), 0);
    historyService->undo();
    observer.receivedEvents.clear();
    
    historyService->redo();
    
    ASSERT_EQ(observer.receivedEvents.size(), 1);
    EXPECT_EQ(observer.receivedEvents[0].key, "Redo");
}

TEST_F(HistoryEventServiceTest, UnsubscribeWorks) {
    observer.stopObserving();
    
    mock.field = 10;
    historyService->recordChange(mock.getFieldByPointer(&mock.field).get(), 0);
    
    EXPECT_EQ(observer.receivedEvents.size(), 0);
}

TEST_F(HistoryEventServiceTest, ActionTriggersChangeRegistered) {
    historyService->recordAction([](){}, [](){});
    
    ASSERT_EQ(observer.receivedEvents.size(), 1);
    EXPECT_EQ(observer.receivedEvents[0].key, "Action");
}

TEST_F(HistoryEventServiceTest, TypeSubscriptionWorks) {
    MockHistoryObserver typeObserver;
    context->injectDependencies(&typeObserver);
    typeObserver.startObservingType<MockInspectableForEvents>();

    // This should trigger typeObserver
    mock.field = 10;
    historyService->recordChange(mock.getFieldByPointer(&mock.field).get(), 0);
    
    // This should NOT trigger typeObserver (wrong type)
    historyService->recordAction([](){}, [](){});
    
    ASSERT_EQ(typeObserver.receivedEvents.size(), 1);
    EXPECT_EQ(typeObserver.receivedEvents[0].instanceType, typeid(mock));
    
    typeObserver.stopObserving();
}

TEST_F(HistoryEventServiceTest, PathSubscriptionWorks) {
    MockHistoryObserver pathObserver;
    context->injectDependencies(&pathObserver);
    pathObserver.startObservingType<MockInspectableForEvents>("/Field");

    // This should trigger pathObserver
    mock.field = 10;
    historyService->recordChange(mock.getFieldByPointer(&mock.field).get(), 0);
    
    // This should NOT trigger pathObserver (wrong path)
    mock.other = 10;
    historyService->recordChange(mock.getFieldByPointer(&mock.other).get(), 0);
    
    ASSERT_EQ(pathObserver.receivedEvents.size(), 1);

    pathObserver.stopObserving();
}

TEST_F(HistoryEventServiceTest, GenericSubscriptionWorks) {
    MockHistoryObserver genericObserver;
    context->injectDependencies(&genericObserver);
    genericObserver.startObservingGeneric();

    // This should trigger genericObserver
    historyService->recordAction([](){}, [](){});
    historyService->undo();
    historyService->redo();
    
    // This should NOT trigger genericObserver
    mock.field = 10;
    historyService->recordChange(mock.getFieldByPointer(&mock.field).get(), 0);
    
    ASSERT_EQ(genericObserver.receivedEvents.size(), 3);
    EXPECT_EQ(genericObserver.receivedEvents[0].key, "Action");
    EXPECT_EQ(genericObserver.receivedEvents[1].key, "Undo");
    EXPECT_EQ(genericObserver.receivedEvents[2].key, "Redo");
    
    genericObserver.stopObserving();
}

TEST_F(HistoryEventServiceTest, TransactionTriggersEvent) {
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
    EXPECT_EQ(observer.receivedEvents[0].instance, &mock);
}
