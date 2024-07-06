//
// Created by vastrakai on 7/5/2024.
//

#pragma once

#include <string>
#include <Utils/MemUtils.hpp>

class Item {
public:
    CLASS_FIELD(short, mItemId, 0xA2);
    CLASS_FIELD(int, mArmorItemType, 0x244);
    CLASS_FIELD(int, mProtection, 0x25C);
    CLASS_FIELD(std::string, mName, 0xD0);
};