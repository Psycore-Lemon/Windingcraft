#include "ui/PauseMenu.h"
#include "ui/VideoSettings.h"

#include <imgui.h>

static const ImVec2 ButtonSize(200, 40);

static ImGuiWindowFlags CenteredWindowFlags()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
                            ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowBgAlpha(0.85f);

    return ImGuiWindowFlags_NoDecoration
         | ImGuiWindowFlags_AlwaysAutoResize
         | ImGuiWindowFlags_NoMove;
}

PauseMenu::Action PauseMenu::Render(Window& window, Config& config, const std::string& configPath)
{
    Action action = Action::None;
    ImGuiWindowFlags flags = CenteredWindowFlags();

    if (currentPage == Page::Main)
    {
        if (ImGui::Begin("Pause", nullptr, flags))
        {
            ImGui::Text("PAUSED");
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button("Resume", ButtonSize))
                action = Action::Resume;

            ImGui::Spacing();

            if (ImGui::Button("Options", ButtonSize))
                currentPage = Page::Options;

            ImGui::Spacing();

            if (ImGui::Button("Quit", ButtonSize))
                action = Action::Quit;
        }
        ImGui::End();
    }
    else if (currentPage == Page::Options)
    {
        if (ImGui::Begin("Options", nullptr, flags))
        {
            ImGui::Text("OPTIONS");
            ImGui::Separator();
            ImGui::Spacing();

            VideoSettings::Render(window, config, configPath);

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button("Back", ButtonSize))
                currentPage = Page::Main;
        }
        ImGui::End();
    }

    return action;
}

void PauseMenu::Reset()
{
    currentPage = Page::Main;
}
