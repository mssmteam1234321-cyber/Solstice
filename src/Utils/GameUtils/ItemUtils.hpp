#pragma once
#include <complex.h>
#include <unordered_map>
#include <SDK/Minecraft/Inventory/Item.hpp>
#include <SDK/Minecraft/Inventory/ItemStack.hpp>

#include <glm/glm.hpp>
//
// Created by vastrakai on 7/6/2024.
//

class ItemUtils {
public:
    static int getItemValue(ItemStack* item);
    static int getBestItem(SItemType type);
    static std::unordered_map<SItemType, int> getBestItems();
    static int getAllPlaceables(bool hotbarOnly = true);
    static int getPlaceableItemOnBlock(glm::vec3 blockPos, bool hotbarOnly = true, bool prioHighest = true);
    static int getSwiftnessSpellbook();
    static void useItem(int slot);
};