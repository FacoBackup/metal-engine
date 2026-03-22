#include <gtest/gtest.h>
#include "../src/editor/dto/Tool.h"
#include <nlohmann/json.hpp>

using namespace Metal;

TEST(ToolPromptTest, MCPFormatTest) {
    Tool tool;
    tool.key = "test_tool";
    tool.description = "A test tool description";
    tool.params = {
        {"param1", "First parameter", ToolParamType::STRING},
        {"param2", "Second parameter", ToolParamType::NUMBER}
    };

    std::string prompt = tool.getSystemPrompt();
    nlohmann::json j = nlohmann::json::parse(prompt);

    EXPECT_EQ(j["name"], "test_tool");
    EXPECT_EQ(j["description"], "A test tool description");
    EXPECT_EQ(j["inputSchema"]["type"], "object");
    EXPECT_EQ(j["inputSchema"]["properties"]["param1"]["type"], "string");
    EXPECT_EQ(j["inputSchema"]["properties"]["param1"]["description"], "First parameter");
    EXPECT_EQ(j["inputSchema"]["properties"]["param2"]["type"], "number");
    EXPECT_EQ(j["inputSchema"]["properties"]["param2"]["description"], "Second parameter");
    
    auto required = j["inputSchema"]["required"];
    EXPECT_TRUE(required.is_array());
    EXPECT_EQ(required.size(), 2);
    EXPECT_EQ(required[0], "param1");
    EXPECT_EQ(required[1], "param2");
}

TEST(ToolPromptTest, NoParamsTest) {
    Tool tool;
    tool.key = "no_param_tool";
    tool.description = "No params description";
    tool.params = {};

    std::string prompt = tool.getSystemPrompt();
    nlohmann::json j = nlohmann::json::parse(prompt);

    EXPECT_EQ(j["name"], "no_param_tool");
    EXPECT_EQ(j["inputSchema"]["type"], "object");
    EXPECT_TRUE(j["inputSchema"]["properties"].empty());
    EXPECT_FALSE(j["inputSchema"].contains("required"));
}
