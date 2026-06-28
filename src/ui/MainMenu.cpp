#include "ui/MainMenu.h"
#include "ui/VideoSettings.h"

#include <imgui.h>
#include <cstdlib>
#include <cstring>
#include <ctime>

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

static int HashString(const char* str)
{
    unsigned int hash = 0;
    for (const char* c = str; *c; ++c)
        hash = hash * 31 + (unsigned char)*c;
    return (int)hash;
}

MainMenu::Action MainMenu::Render(Window& window, Config& config, const std::string& configPath)
{
    Action action = Action::None;
    ImGuiWindowFlags flags = CenteredWindowFlags();

    if (currentPage == Page::Main)
    {
        if (ImGui::Begin("MainMenu", nullptr, flags))
        {
            ImGui::Text("WINDINGCRAFT");
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button("New World", ButtonSize))
            {
                snprintf(seedInput, sizeof(seedInput), "%d", (int)(std::time(nullptr) % 100000));
                snprintf(nameInput, sizeof(nameInput), "My World");
                currentPage = Page::NewWorld;
            }

            ImGui::Spacing();

            if (ImGui::Button("Load World", ButtonSize))
            {
                saveList = SaveManager::ListSaves();
                currentPage = Page::LoadWorld;
            }

            ImGui::Spacing();

            if (ImGui::Button("Options", ButtonSize))
                currentPage = Page::Options;

            ImGui::Spacing();

            if (ImGui::Button("Quit", ButtonSize))
                action = Action::Quit;
        }
        ImGui::End();
    }
    else if (currentPage == Page::NewWorld)
    {
        if (ImGui::Begin("NewWorld", nullptr, flags))
        {
            ImGui::Text("NEW WORLD");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Text("World Name:");
            ImGui::SetNextItemWidth(200);
            ImGui::InputText("##name", nameInput, sizeof(nameInput));

            ImGui::Spacing();

            ImGui::Text("Seed:");
            ImGui::SetNextItemWidth(200);
            ImGui::InputText("##seed", seedInput, sizeof(seedInput));

            ImGui::Spacing();

            if (ImGui::Button("Create World", ButtonSize))
            {
                int seed = std::atoi(seedInput);
                if (seed == 0 && std::strlen(seedInput) > 0)
                    seed = HashString(seedInput);

                worldData.name = nameInput;
                worldData.seed = seed;
                worldData.playerPosition = glm::vec3(0.0f, 20.0f, 0.0f);

                action = Action::Play;
                currentPage = Page::Main;
            }

            ImGui::Spacing();

            if (ImGui::Button("Back", ButtonSize))
                currentPage = Page::Main;
        }
        ImGui::End();
    }
    else if (currentPage == Page::LoadWorld)
    {
        if (ImGui::Begin("LoadWorld", nullptr, flags))
        {
            ImGui::Text("LOAD WORLD");
            ImGui::Separator();
            ImGui::Spacing();

            if (saveList.empty())
            {
                ImGui::TextDisabled("No saved worlds found.");
            }
            else
            {
                for (const auto& save : saveList)
                {
                    if (ImGui::Button(save.c_str(), ButtonSize))
                    {
                        if (SaveManager::Load(save, worldData))
                        {
                            action = Action::Play;
                            currentPage = Page::Main;
                        }
                    }
                    ImGui::Spacing();
                }
            }

            ImGui::Spacing();

            if (ImGui::Button("Back", ButtonSize))
                currentPage = Page::Main;
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

const SaveData& MainMenu::GetWorldData() const
{
    return worldData;
}
