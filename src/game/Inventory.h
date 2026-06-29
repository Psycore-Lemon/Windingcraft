#pragma once

#include "game/BlockType.h"

struct InventorySlot
{
    BlockType type = BlockType::Air;
    int count = 0;

    bool IsEmpty() const { return type == BlockType::Air || count <= 0; }
};

class Inventory
{
public:
    static constexpr int HOTBAR_SIZE = 9;

    Inventory();

    const InventorySlot& GetSlot(int index) const;
    void SetSlot(int index, BlockType type, int count);

    int GetSelectedIndex() const;
    void SetSelectedIndex(int index);
    void ScrollSelection(int direction);

    BlockType GetSelectedType() const;

private:
    InventorySlot hotbar[HOTBAR_SIZE];
    int selectedIndex = 0;
};
