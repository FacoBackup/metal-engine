#include <gtest/gtest.h>
#include "../src/common/IEventMember.h"
#include "../src/ApplicationEventContext.h"
#include <vector>
#include <memory>

using namespace Metal;

class TestEventMember : public IEventMember {
public:
    std::vector<Event> receivedEvents;

    void startObserving(const std::string &key) {
        eventListener([this](const Event &event) {
            receivedEvents.push_back(event);
        }, key);
    }
};

class ApplicationEventContextTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

TEST_F(ApplicationEventContextTest, DispatchToSingleSubscriber) {
    TestEventMember member;
    member.startObserving("EventA");

    auto payload = std::make_shared<EventPayload>();
    ApplicationEventContext::dispatch("EventA", payload);

    ASSERT_EQ(member.receivedEvents.size(), 1);
    EXPECT_EQ(member.receivedEvents[0].key, "EventA");
    EXPECT_EQ(member.receivedEvents[0].payload, payload);
}

TEST_F(ApplicationEventContextTest, DispatchToMultipleSubscribers) {
    TestEventMember member1;
    TestEventMember member2;
    member1.startObserving("EventA");
    member2.startObserving("EventA");

    ApplicationEventContext::dispatch("EventA");

    EXPECT_EQ(member1.receivedEvents.size(), 1);
    EXPECT_EQ(member2.receivedEvents.size(), 1);
}

TEST_F(ApplicationEventContextTest, DispatchToSpecificKeyOnly) {
    TestEventMember member;
    member.startObserving("EventA");

    ApplicationEventContext::dispatch("EventB");

    EXPECT_EQ(member.receivedEvents.size(), 0);

    ApplicationEventContext::dispatch("EventA");
    EXPECT_EQ(member.receivedEvents.size(), 1);
}

TEST_F(ApplicationEventContextTest, AutomaticUnsubscribeOnDestruction) {
    {
        TestEventMember member;
        member.startObserving("EventA");
        ApplicationEventContext::dispatch("EventA");
        ASSERT_EQ(member.receivedEvents.size(), 1);
    } // member is destroyed here, should unsubscribe

    TestEventMember member2;
    member2.startObserving("EventA");
    ApplicationEventContext::dispatch("EventA");
    EXPECT_EQ(member2.receivedEvents.size(), 1);
}

TEST_F(ApplicationEventContextTest, MultiEventListener) {
    TestEventMember member;
    member.eventListener([&](const Event &e) {
        member.receivedEvents.push_back(e);
    }, "Event1", "Event2", "Event3");

    ApplicationEventContext::dispatch("Event1");
    ApplicationEventContext::dispatch("Event2");
    ApplicationEventContext::dispatch("Event3");

    EXPECT_EQ(member.receivedEvents.size(), 3);
    EXPECT_EQ(member.receivedEvents[0].key, "Event1");
    EXPECT_EQ(member.receivedEvents[1].key, "Event2");
    EXPECT_EQ(member.receivedEvents[2].key, "Event3");
}
