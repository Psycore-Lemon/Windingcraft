#include "ui/HUD.h"

#include <imgui.h>

void HUD::RenderOverlay(const glm::vec3& playerPos, bool grounded, bool flying,
                        int chunkCount, const char* lookingAt)
{
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.5f);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration
                           | ImGuiWindowFlags_AlwaysAutoResize
                           | ImGuiWindowFlags_NoFocusOnAppearing
                           | ImGuiWindowFlags_NoNav;

    if (ImGui::Begin("HUD", nullptr, flags))
    {
        ImGui::Text("FPS: %.0f", ImGui::GetIO().Framerate);
        ImGui::Separator();
        ImGui::Text("Pos: %.1f, %.1f, %.1f", playerPos.x, playerPos.y, playerPos.z);
        ImGui::Text("Mode: %s", flying ? "Flying" : (grounded ? "Grounded" : "Airborne"));
        ImGui::Text("Chunks: %d", chunkCount);
        ImGui::Separator();
        ImGui::Text("Looking at: %s", lookingAt);
    }
    ImGui::End();
}

void HUD::RenderCrosshair()
{
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 center(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);

    ImDrawList* draw = ImGui::GetForegroundDrawList();
    ImU32 color = IM_COL32(255, 255, 255, 200);
    draw->AddLine(ImVec2(center.x - 10, center.y), ImVec2(center.x + 10, center.y), color, 2.0f);
    draw->AddLine(ImVec2(center.x, center.y - 10), ImVec2(center.x, center.y + 10), color, 2.0f);
}
