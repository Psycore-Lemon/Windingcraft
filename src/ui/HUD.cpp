#include "ui/HUD.h"
#include "scene/PlayerStatus.h"
#include "game/BlockType.h"

#include <imgui.h>
#include <cstdio>

static void DrawResourceBar(ImDrawList* draw, ImVec2 pos, float width, float height,
                            float ratio, ImU32 fillColor, const char* label)
{
    ImVec2 bgMin = pos;
    ImVec2 bgMax(pos.x + width, pos.y + height);
    draw->AddRectFilled(bgMin, bgMax, IM_COL32(20, 20, 20, 200), 3.0f);

    float fillWidth = width * ratio;
    if (fillWidth > 0.0f)
    {
        ImVec2 fillMax(pos.x + fillWidth, pos.y + height);
        draw->AddRectFilled(pos, fillMax, fillColor, 3.0f);
    }

    draw->AddRect(bgMin, bgMax, IM_COL32(60, 60, 60, 200), 3.0f);

    ImVec2 textSize = ImGui::CalcTextSize(label);
    ImVec2 textPos(pos.x + (width - textSize.x) * 0.5f, pos.y + (height - textSize.y) * 0.5f);
    draw->AddText(textPos, IM_COL32(255, 255, 255, 220), label);
}

void HUD::RenderOverlay(const PlayerStatus& status, int chunkCount)
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
        if (status.ping >= 0)
            ImGui::Text("Ping: %d ms", status.ping);
        ImGui::Separator();
        ImGui::Text("Pos: %.1f, %.1f, %.1f", status.position.x, status.position.y, status.position.z);
        ImGui::Text("Mode: %s", status.flying ? "Flying" : (status.grounded ? "Grounded" : "Airborne"));
        ImGui::Text("Chunks: %d", chunkCount);
        ImGui::Separator();
        const char* lookName = status.hasTarget
            ? Blocks::Get(status.lookingAtBlock).name : "---";
        ImGui::Text("Looking at: %s", lookName);
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

void HUD::RenderResourceBars(const PlayerStatus& status)
{
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* draw = ImGui::GetForegroundDrawList();

    float barWidth = 200.0f;
    float barHeight = 18.0f;
    float spacing = 4.0f;
    float margin = 10.0f;

    float startX = margin;
    float startY = io.DisplaySize.y - margin - 3 * (barHeight + spacing) - 80.0f;

    const auto& v = status.vitals;

    char label[32];

    snprintf(label, sizeof(label), "HP  %.0f / %.0f", v.health.current, v.health.max);
    DrawResourceBar(draw, ImVec2(startX, startY),
                    barWidth, barHeight, v.health.Ratio(),
                    IM_COL32(200, 40, 40, 220), label);

    snprintf(label, sizeof(label), "MP  %.0f / %.0f", v.mana.current, v.mana.max);
    DrawResourceBar(draw, ImVec2(startX, startY + barHeight + spacing),
                    barWidth, barHeight, v.mana.Ratio(),
                    IM_COL32(40, 80, 200, 220), label);

    snprintf(label, sizeof(label), "SP  %.0f / %.0f", v.stamina.current, v.stamina.max);
    DrawResourceBar(draw, ImVec2(startX, startY + 2 * (barHeight + spacing)),
                    barWidth, barHeight, v.stamina.Ratio(),
                    IM_COL32(40, 180, 40, 220), label);
}
