#pragma once

#include <imgui.h>

namespace UIHelpers
{
    inline void BeginCenteredWindow(const char* name, const ImVec2& size, float alpha = 0.9f)
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
                                ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(size);
        ImGui::SetNextWindowBgAlpha(alpha);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;
        ImGui::Begin(name, nullptr, flags);
    }

    inline void CenteredText(const char* text)
    {
        float width = ImGui::CalcTextSize(text).x;
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - width) * 0.5f);
        ImGui::Text("%s", text);
    }

    inline bool CenteredButton(const char* label, const ImVec2& size)
    {
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - size.x) * 0.5f);
        return ImGui::Button(label, size);
    }
}
