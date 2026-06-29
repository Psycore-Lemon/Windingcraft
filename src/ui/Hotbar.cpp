#include "ui/Hotbar.h"
#include "game/Inventory.h"

#include <imgui.h>
#include <cstdio>

void Hotbar::Render(const Inventory& inventory)
{
    ImGuiIO& io = ImGui::GetIO();

    float slotSize = 50.0f;
    float padding = 4.0f;
    float totalWidth = Inventory::HOTBAR_SIZE * (slotSize + padding) - padding;
    float startX = (io.DisplaySize.x - totalWidth) * 0.5f;
    float startY = io.DisplaySize.y - slotSize - 20.0f;

    ImDrawList* draw = ImGui::GetForegroundDrawList();

    int selected = inventory.GetSelectedIndex();

    for (int i = 0; i < Inventory::HOTBAR_SIZE; ++i)
    {
        float x = startX + i * (slotSize + padding);
        float y = startY;

        ImVec2 min(x, y);
        ImVec2 max(x + slotSize, y + slotSize);

        draw->AddRectFilled(min, max, IM_COL32(30, 30, 30, 200), 4.0f);

        if (i == selected)
            draw->AddRect(min, max, IM_COL32(255, 255, 255, 255), 4.0f, 0, 2.5f);
        else
            draw->AddRect(min, max, IM_COL32(80, 80, 80, 200), 4.0f);

        const InventorySlot& slot = inventory.GetSlot(i);

        if (!slot.IsEmpty())
        {
            const BlockDef& def = Blocks::Get(slot.type);

            ImU32 blockColor = IM_COL32(
                (int)(def.r * 255), (int)(def.g * 255), (int)(def.b * 255), 255);

            float inset = 8.0f;
            draw->AddRectFilled(
                ImVec2(x + inset, y + inset),
                ImVec2(x + slotSize - inset, y + slotSize - inset),
                blockColor, 2.0f);

            char countStr[8];
            snprintf(countStr, sizeof(countStr), "%d", slot.count);
            ImVec2 textSize = ImGui::CalcTextSize(countStr);
            draw->AddText(
                ImVec2(x + slotSize - textSize.x - 4, y + slotSize - textSize.y - 2),
                IM_COL32(255, 255, 255, 220), countStr);

            draw->AddText(
                ImVec2(x + 4, y + 2),
                IM_COL32(200, 200, 200, 180), def.name);
        }

        char keyStr[4];
        snprintf(keyStr, sizeof(keyStr), "%d", (i + 1) % 10);
        draw->AddText(
            ImVec2(x + slotSize - ImGui::CalcTextSize(keyStr).x - 4, y + 2),
            IM_COL32(120, 120, 120, 150), keyStr);
    }
}
