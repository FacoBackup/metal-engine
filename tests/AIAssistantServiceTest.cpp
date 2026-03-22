#include <gtest/gtest.h>
#include <entt/entt.hpp>
#include "../src/editor/service/AIAssistantService.h"
#include "../src/editor/repository/AIAssistantRepository.h"
#include "../src/editor/repository/EditorRepository.h"
#include "../src/editor/service/HttpService.h"
#include "../src/editor/service/NotificationService.h"
#include "../src/core/DirectoryService.h"
#include "../src/ApplicationContext.h"
#include "../src/editor/dto/IToolProvider.h"
#include <nlohmann/json.hpp>

#include "common/LoggerUtil.h"

using namespace Metal;

class TestToolProvider : public IToolProvider {
public:
    bool toolExecuted = false;

    void registerTools() override {
        registerTool("PLACEHOLDER_TOOL_KEY", "A placeholder tool for testing real AI integration",
            {{"arg1", "Test argument", ToolParamType::STRING}},
            [this](const nlohmann::json& args) {
                toolExecuted = true;
                LOG_INFO("Executing tool");
                return "Successfully executed tool with arg: " + args.value("arg1", "none");
            });
    }
};

class AIAssistantServiceIntegrationTest : public ::testing::Test {
protected:
    std::shared_ptr<ApplicationContext> ctx;
    std::shared_ptr<AIAssistantService> service;
    std::shared_ptr<AIAssistantRepository> aiRepo;
    std::shared_ptr<EditorRepository> editorRepo;
    std::shared_ptr<HttpService> httpService;
    std::shared_ptr<NotificationService> notificationService;
    std::shared_ptr<DirectoryService> directoryService;
    std::shared_ptr<TestToolProvider> toolProvider;

    void SetUp() override {
        ctx = std::make_shared<ApplicationContext>(true);
        
        aiRepo = std::make_shared<AIAssistantRepository>();
        editorRepo = std::make_shared<EditorRepository>();
        httpService = std::make_shared<HttpService>();
        notificationService = std::make_shared<NotificationService>();
        directoryService = std::make_shared<DirectoryService>();
        toolProvider = std::make_shared<TestToolProvider>();

        ctx->registerSingleton<AIAssistantRepository>(aiRepo);
        ctx->registerSingleton<EditorRepository>(editorRepo);
        ctx->registerSingleton<HttpService>(httpService);
        ctx->registerSingleton<NotificationService>(notificationService);
        ctx->registerSingleton<DirectoryService>(directoryService);
        ctx->registerSingleton<IToolProvider>(toolProvider);

        service = std::make_shared<AIAssistantService>();
        ctx->registerSingleton<AIAssistantService>(service);
        
        ctx->injectDependencies(service.get());
        service->onInitialize();

        editorRepo->geminiMcpKey = "KEY";
    }
};

TEST_F(AIAssistantServiceIntegrationTest, TestRealToolUsageWithGeminiFlashLite) {
    auto chat = aiRepo->createNewChat();
    std::string chatId = chat->id;
    service->sendRequest(chatId, "Please use the tool 'PLACEHOLDER_TOOL_KEY' with arg1 set to 'integration-test'", AIModel::GEMINI_3_FLASH_LITE);
    
    // Wait for the AI to process (timeout after 30 seconds)
    int waitCounter = 0;
    while (chat->isProcessing && waitCounter < 30) {
        _sleep(1000);
        waitCounter++;
    }
    
    ASSERT_FALSE(chat->isProcessing) << "AI timed out or failed to finish processing";
    ASSERT_TRUE(toolProvider->toolExecuted) << "The tool was never executed by the AI strategy";
    
    // Check messages
    // Message 0: User message
    // Message 1: Assistant message (the one that should have the tool call)
    ASSERT_GE(chat->messages.size(), 2);
    
    const auto& assistantMsg = chat->messages[1];
    EXPECT_EQ(assistantMsg.role, "assistant");
    EXPECT_FALSE(assistantMsg.toolCalls.empty()) << "Assistant message should have at least one tool call";
    
    if (!assistantMsg.toolCalls.empty()) {
        EXPECT_EQ(assistantMsg.toolCalls[0].name, "PLACEHOLDER_TOOL_KEY");
        EXPECT_FALSE(assistantMsg.toolCalls[0].result.empty()) << "Tool call result should not be empty";
    }

    // No assistant message should contain JSON parts if a tool call occurred
    for (const auto& msg : chat->messages) {
        if (msg.role == "assistant") {
            EXPECT_EQ(msg.content.find("{"), std::string::npos) << "Assistant message content should not contain JSON: " << msg.content;
            EXPECT_EQ(msg.content.find("}"), std::string::npos) << "Assistant message content should not contain JSON: " << msg.content;
        }
    }
}
