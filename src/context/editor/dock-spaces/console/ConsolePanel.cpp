#include "ConsolePanel.h"
#include "../../../../util/Logger.h""
#include "imgui.h"

namespace Metal {
    namespace
    {
        const ImVec4 INFO_COLOR{0.f, 0.f, 1.f, 1.0f};
        const ImVec4 WARNING_COLOR{1.0f, 1.0f, 0.0f, 1.0f};
        const ImVec4 ERROR_COLOR{1.0f, 0.0f, 0.0f, 1.0f};
    }

    void ConsolePanel::onSync()
    {
        ImGui::Checkbox("Info", &showInfo);
        ImGui::SameLine();
        ImGui::Checkbox("Warning", &showWarning);
        ImGui::SameLine();
        ImGui::Checkbox("Error", &showError);
        ImGui::SameLine();
        ImGui::InputText("Search", searchBuffer, sizeof(searchBuffer));

        ImGui::BeginChild("ConsoleLog", ImVec2(0, 0), true);
        const auto& logs = Logger::logStorage->getLogs();
        int index = 0;
        for (const auto& log : logs)
        {
            if (index >= Logger::logStorage->currentIndex)
            {
                continue;
            }
            index++;
            bool showLog = (showInfo && log.level == LogLevel::Info) ||
                (showWarning && log.level == LogLevel::Warning) ||
                (showError && log.level == LogLevel::Error);

            if (!showLog) continue;

            if (searchBuffer[0] != '\0' &&
                log.message.find(searchBuffer) == std::string::npos)
                continue;

            const char* levelPrefix;
            ImVec4 levelColor;

            switch (log.level)
            {
            case LogLevel::Info:
                levelPrefix = "[INFO] ";
                levelColor = INFO_COLOR;
                break;
            case LogLevel::Warning:
                levelPrefix = "[WARN] ";
                levelColor = WARNING_COLOR;
                break;
            case LogLevel::Error:
                levelPrefix = "[ERROR] ";
                levelColor = ERROR_COLOR;
                break;
            }

            auto timePoint = log.timestamp;
            auto time = std::chrono::system_clock::to_time_t(timePoint);
            auto localTime = *std::localtime(&time);
            char timestamp[32];
            std::strftime(timestamp, sizeof(timestamp), "%H:%M:%S", &localTime);

            ImGui::TextUnformatted(timestamp);
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Text, levelColor);
            ImGui::TextUnformatted(levelPrefix);
            ImGui::PopStyleColor();

            ImGui::SameLine();
            ImGui::TextUnformatted(log.message.c_str());
        }
        ImGui::EndChild();
    }
} // Metal
