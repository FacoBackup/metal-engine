#ifndef AIMODEL_H
#define AIMODEL_H

#include <string>
#include <vector>

namespace Metal {
    enum class AIModel {
        // OpenAI - Modern & Cheap
        GPT_5_NANO,     // Ultra-low cost for high-volume tasks
        GPT_5_MINI,     // Best balance of reasoning and price
        GPT_5_PRO,      // Flagship reasoning model

        // Google Gemini - Modern & Cheap
        GEMINI_3_FLASH_LITE, // Fastest and most budget-friendly
        GEMINI_3_FLASH,      // High-performance at Flash pricing
        GEMINI_3_1_PRO       // Advanced reasoning and long context
    };

    struct AIModelInfo {
        AIModel model;
        std::string name;
        std::string url;
        std::string modelId;
    };

    inline AIModelInfo getAIModelInfo(AIModel model) {
        switch (model) {
            // OpenAI Models
            case AIModel::GPT_5_NANO:
                return {AIModel::GPT_5_NANO, "GPT-5 Nano", "https://api.openai.com/v1", "gpt-5-nano"};
            case AIModel::GPT_5_MINI:
                return {AIModel::GPT_5_MINI, "GPT-5 Mini", "https://api.openai.com/v1", "gpt-5-mini"};
            case AIModel::GPT_5_PRO:
                return {AIModel::GPT_5_PRO, "GPT-5 Pro", "https://api.openai.com/v1", "gpt-5-pro"};

            // Gemini Models
            case AIModel::GEMINI_3_FLASH_LITE:
                return {AIModel::GEMINI_3_FLASH_LITE, "Gemini 3.1 Flash-Lite", "https://generativelanguage.googleapis.com/v1beta", "gemini-3.1-flash-lite-preview"};
            case AIModel::GEMINI_3_FLASH:
                return {AIModel::GEMINI_3_FLASH, "Gemini 3 Flash", "https://generativelanguage.googleapis.com/v1beta", "gemini-3-flash-preview"};
            case AIModel::GEMINI_3_1_PRO:
                return {AIModel::GEMINI_3_1_PRO, "Gemini 3.1 Pro", "https://generativelanguage.googleapis.com/v1beta", "gemini-3.1-pro-preview"};

            default:
                return {AIModel::GPT_5_MINI, "GPT-5 Mini", "https://api.openai.com/v1", "gpt-5-mini"};
        }
    }

    inline std::vector<AIModel> getAllAIModels() {
        return {
            AIModel::GPT_5_NANO,
            AIModel::GPT_5_MINI,
            AIModel::GPT_5_PRO,
            AIModel::GEMINI_3_FLASH_LITE,
            AIModel::GEMINI_3_FLASH,
            AIModel::GEMINI_3_1_PRO
        };
    }
}

#endif // AIMODEL_H