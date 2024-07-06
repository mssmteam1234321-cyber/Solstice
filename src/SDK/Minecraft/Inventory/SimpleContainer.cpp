//
// Created by vastrakai on 7/6/2024.
//

#include "SimpleContainer.hpp"

#include <SDK/OffsetProvider.hpp>
#include <Utils/MemUtils.hpp>

ItemStack* Container::getItem(int slot)
{
    static auto vFunc = OffsetProvider::Container_getItem;
    return MemUtils::callVirtualFunc<ItemStack*, int>(vFunc, this, slot);
}
