#pragma once
//
// Created by vastrakai on 7/5/2024.
//

#include <Utils/MemUtils.hpp>

class Inventory {
public:
    virtual ~Inventory();
    class ItemStack* getItem(int);
};

class PlayerInventory {
public:
    CLASS_FIELD(int, mSelectedSlot, 0x10);

    Inventory* getContainer();
};