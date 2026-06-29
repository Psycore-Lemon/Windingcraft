#include "ui/MainMenu.h"
#include "ui/UIHelpers.h"
#include "ui/VideoSettings.h"

#include <imgui.h>
#include <cstdlib>
#include <cstring>
#include <ctime>

using namespace UIHelpers;

static const ImVec2 ButtonSize(280, 40);
static const ImVec2 MenuSize(320, 300);
static const ImVec2 WorldListSize(500, 400);

static int HashString(const char* str)
{
    unsigned int hash = 0;
    for (const char* c = str; *c; ++c)
        hash = hash * 31 + (unsigned char)*c;
    return (int)hash;
}

void MainMenu::RefreshSaveList()
{
    saveList = SaveManager::ListSaves();
}

MainMenu::Action MainMenu::Render(Window& window, Config& config, const std::string& configPath)
{
    Action action = Action::None;

    if (currentPage == Page::Main)
    {
        BeginCenteredWindow("MainMenu", MenuSize);

        ImGui::Spacing();
        ImGui::Spacing();
        CenteredText("WINDINGCRAFT");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Spacing();

        if (CenteredButton("Singleplayer", ButtonSize))
        {
            RefreshSaveList();
            pendingDelete.clear();
            currentPage = Page::Singleplayer;
        }

        ImGui::Spacing();

        if (CenteredButton("Options", ButtonSize))
            currentPage = Page::Options;

        ImGui::Spacing();

        if (CenteredButton("Quit", ButtonSize))
            action = Action::Quit;

        ImGui::End();
    }
    else if (currentPage == Page::Singleplayer)
    {
        BeginCenteredWindow("Singleplayer", WorldListSize);

        ImGui::Spacing();
        CenteredText("WORLDS");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        float listHeight = ImGui::GetContentRegionAvail().y - 60;

        if (ImGui::BeginChild("WorldList", ImVec2(0, listHeight), true))
        {
            if (saveList.empty())
            {
                ImGui::Spacing();
                CenteredText("No worlds yet. Create one below!");
                ImGui::Spacing();
            }
            else
            {
                float columnWidth = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

                ImGui::Columns(2, "worlds", false);
                ImGui::SetColumnWidth(0, columnWidth + ImGui::GetStyle().ItemSpacing.x * 0.5f);
                ImGui::SetColumnWidth(1, columnWidth + ImGui::GetStyle().ItemSpacing.x * 0.5f);

                for (int i = 0; i < (int)saveList.size(); ++i)
                {
                    const auto& save = saveList[i];
                    ImGui::PushID(i);

                    float cardWidth = ImGui::GetColumnWidth() - ImGui::GetStyle().ItemSpacing.x;

                    ImGui::BeginGroup();
                    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 4.0f);

                    if (ImGui::BeginChild("card", ImVec2(cardWidth, 70), true))
                    {
                        ImGui::Text("%s", save.c_str());
                        ImGui::Spacing();

                        if (ImGui::Button("Play", ImVec2(60, 25)))
                        {
                            if (SaveManager::Load(save, worldData))
                            {
                                action = Action::Play;
                                currentPage = Page::Main;
                            }
                        }

                        ImGui::SameLine();

                        if (pendingDelete == save)
                        {
                            if (ImGui::Button("Yes", ImVec2(35, 25)))
                            {
                                SaveManager::Delete(save);
                                pendingDelete.clear();
                                RefreshSaveList();
                                ImGui::EndChild();
                                ImGui::PopStyleVar();
                                ImGui::EndGroup();
                                ImGui::PopID();
                                ImGui::Columns(1);
                                goto end_list;
                            }
                            ImGui::SameLine();
                            if (ImGui::Button("No", ImVec2(35, 25)))
                                pendingDelete.clear();
                        }
                        else
                        {
                            if (ImGui::Button("Delete", ImVec2(60, 25)))
                                pendingDelete = save;
                        }
                    }
                    ImGui::EndChild();
                    ImGui::PopStyleVar();
                    ImGui::EndGroup();

                    ImGui::Spacing();

                    ImGui::NextColumn();
                    ImGui::PopID();
                }

                ImGui::Columns(1);
            }
        }
        end_list:
        ImGui::EndChild();

        ImGui::Spacing();

        float bottomWidth = ImGui::GetContentRegionAvail().x;
        float btnW = 140;
        float spacing = 10;
        float startX = (bottomWidth - btnW * 2 - spacing) * 0.5f;

        ImGui::SetCursorPosX(startX);
        if (ImGui::Button("New World", ImVec2(btnW, 35)))
        {
            snprintf(seedInput, sizeof(seedInput), "%d", (int)(std::time(nullptr) % 100000));
            snprintf(nameInput, sizeof(nameInput), "My World");
            currentPage = Page::NewWorld;
        }

        ImGui::SameLine(0, spacing);

        if (ImGui::Button("Back", ImVec2(btnW, 35)))
            currentPage = Page::Main;

        ImGui::End();
    }
    else if (currentPage == Page::NewWorld)
    {
        BeginCenteredWindow("NewWorld", ImVec2(350, 280));

        ImGui::Spacing();
        CenteredText("NEW WORLD");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("World Name:");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##name", nameInput, sizeof(nameInput));

        ImGui::Spacing();

        ImGui::Text("Seed:");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##seed", seedInput, sizeof(seedInput));

        ImGui::Spacing();
        ImGui::Spacing();

        if (CenteredButton("Create World", ButtonSize))
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

        if (CenteredButton("Back", ButtonSize))
            currentPage = Page::Singleplayer;

        ImGui::End();
    }
    else if (currentPage == Page::Options)
    {
        BeginCenteredWindow("Options", ImVec2(350, 350));

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

const SaveData& MainMenu::GetWorldData() const
{
    return worldData;
}
