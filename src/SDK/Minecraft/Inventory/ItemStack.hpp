//
// Created by vastrakai on 7/5/2024.
//

#pragma once

#include <SDK/SigManager.hpp>
#include <Utils/MemUtils.hpp>

class ItemStackBase {
public:
    uintptr_t** mVfTable;
    //WeakPtr<class Item> mItem;
    class Item** mItem;
    void* mCompoundTag;
    class Block* mBlock;
    short mAuxValue;
    int8_t mCount;
    bool valid;
    PAD(0x60);

    Item* getItem() {
        return *mItem;
    }
};

enum class Enchant : int {
    PROTECTION = 0,
    FIRE_PROTECTION = 1,
    FEATHER_FALLING = 2,
    BLAST_PROTECTION = 3,
    PROJECTILE_PROTECTION = 4,
    THORNS = 5,
    RESPIRATION = 6,
    DEPTH_STRIDER = 7,
    AQUA_AFFINITY = 8,
    SHARPNESS = 9,
    SMITE = 10,
    BANE_OF_ARTHROPODS = 11,
    KNOCKBACK = 12,
    FIRE_ASPECT = 13,
    LOOTING = 14,
    EFFICIENCY = 15,
    SILK_TOUCH = 16,
    UNBREAKING = 17,
    FORTUNE = 18,
    POWER = 19,
    PUNCH = 20,
    FLAME = 21,
    BOW_INFINITY = 22,
    LUCK_OF_THE_SEA = 23,
    LURE = 24,
    FROST_WALKER = 25,
    MENDING = 26,
    BINDING = 27,
    VANISHING = 28,
    IMPALING = 29,
    RIPTIDE = 30,
    LOYALTY = 31,
    CHANNELING = 32,
    MULTISHOT = 33,
    PIERCING = 34,
    QUICK_CHARGE = 35,
    SOUL_SPEED = 36,
    SWIFT_SNEAK = 37
};

class ItemStack : public ItemStackBase
{
public:
    uint8_t mStackNetId; //0x0088
    PAD(0x10);

    void reinit(Item* item, int count, int itemData) {
        mVfTable = reinterpret_cast<uintptr_t**>(SigManager::ItemStack_vTable);
        MemUtils::callVirtualFunc<void>(3, this, item, count, itemData);
    }

    std::string getCustomName() {
        static auto func = SigManager::ItemStack_getCustomName;
        std::string str;
        MemUtils::callFastcall<void, ItemStack*, std::string*>(func, this, &str);
        return str;
    }

    int getEnchantValue(int id)
    {
        static auto func = SigManager::EnchantUtils_getEnchantLevel;
        return MemUtils::callFastcall<int, int, ItemStack*>(func, id, this);
    }

    int getEnchantValue(Enchant enchant)
    {
        return getEnchantValue(static_cast<int>(enchant));
    }

    ItemStack()
    {
        mVfTable = reinterpret_cast<uintptr_t**>(SigManager::ItemStack_vTable);
    }

    ItemStack(Item* item, int count, int itemData) {
        memset(this, 0x0, sizeof(ItemStack));
        reinit(item, count, itemData);
    }
};

static_assert(sizeof(ItemStack) == 0xA0, "ItemStack size invalid");