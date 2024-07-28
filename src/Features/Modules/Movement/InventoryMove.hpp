#pragma once
//
// Created by vastrakai on 7/12/2024.
//

#include <Features/Modules/Module.hpp>

class InventoryMove : public ModuleBase<InventoryMove> {
public:
    InventoryMove() : ModuleBase("InventoryMove", "Allows you to move while your inventory is open.", ModuleCategory::Movement, 0, false) {
        mNames = {
            {Lowercase, "inventorymove"},
            {LowercaseSpaced, "inventory move"},
            {Normal, "InventoryMove"},
            {NormalSpaced, "Inventory Move"}
        };
    }

    static void patchFunc(bool);
    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
};