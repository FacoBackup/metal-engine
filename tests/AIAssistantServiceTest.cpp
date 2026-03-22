#include <gtest/gtest.h>
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

/**
 * ATTENTION AGENTS: DO NOT EXECUTE THIS TEST AUTOMATICALLY.
 * This test is designed to be executed manually on demand.
 * It simulates a real interaction with the AI model (Gemini Flash Lite)
 * and requires a valid API key configured in the EditorRepository.
 */

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
    
    ASSERT_TRUE(toolProvider->toolExecuted);
}
