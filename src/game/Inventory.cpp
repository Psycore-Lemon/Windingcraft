#include "game/Inventory.h"

Inventory::Inventory()
{
    hotbar[0] = { BlockType::Dirt, GameConfig::DefaultStackSize };
    hotbar[1] = { BlockType::Stone, GameConfig::DefaultStackSize };
}

const InventorySlot& Inventory::GetSlot(int index) const
{
    return hotbar[index];
}

void Inventory::SetSlot(int index, BlockType type, int count)
{
    if (index >= 0 && index < HOTBAR_SIZE)
        hotbar[index] = { type, count };
}

int Inventory::GetSelectedIndex() const
{
    return selectedIndex;
}

void Inventory::SetSelectedIndex(int index)
{
    if (index < 0)
        index = HOTBAR_SIZE - 1;
    else if (index >= HOTBAR_SIZE)
        index = 0;

    selectedIndex = index;
}

void Inventory::ScrollSelection(int direction)
{
    SetSelectedIndex(selectedIndex - direction);
}

BlockType Inventory::GetSelectedType() const
{
    return hotbar[selectedIndex].type;
}
