#include "ui/PauseMenu.h"
#include "ui/UIHelpers.h"
#include "ui/VideoSettings.h"

#include <imgui.h>

using namespace UIHelpers;

static const ImVec2 ButtonSize(200, 40);
static const ImVec2 PauseSize(240, 320);
static const ImVec2 OptionsSize(350, 350);

PauseMenu::Action PauseMenu::Render(Window& window, Config& config, const std::string& configPath)
{
    Action action = Action::None;

    if (currentPage == Page::Main)
    {
        BeginCenteredWindow("Pause", PauseSize, 0.85f);

        ImGui::Spacing();
        CenteredText("PAUSED");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (CenteredButton("Resume", ButtonSize))
            action = Action::Resume;

        ImGui::Spacing();

        if (CenteredButton("Options", ButtonSize))
            currentPage = Page::Options;

        ImGui::Spacing();

        if (CenteredButton("Quit to Menu", ButtonSize))
            action = Action::QuitToMenu;

        ImGui::Spacing();

        if (CenteredButton("Quit to Desktop", ButtonSize))
            action = Action::QuitToDesktop;

        ImGui::End();
    }
    else if (currentPage == Page::Options)
    {
        BeginCenteredWindow("Options", OptionsSize);

        ImGui::Spacing();
        CenteredText("OPTIONS");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        VideoSettings::Render(window, config, configPath);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (CenteredButton("Back", ButtonSize))
            currentPage = Page::Main;

        ImGui::End();
    }

    return action;
}

void PauseMenu::Reset()
{
    currentPage = Page::Main;
}
