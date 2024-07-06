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
    PAD(0x60);
};

class ItemStack : public ItemStackBase {
public:
    uint8_t mStackNetId = 0; //0x0088
    char pad_0089[23]; //0x0089

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

    ItemStack()
    {
        mVfTable = reinterpret_cast<uintptr_t**>(SigManager::ItemStack_vTable);
    }
};