//
// Created by vastrakai on 6/29/2024.
//

#include "Keyboard.hpp"

#include "StringUtils.hpp"

int Keyboard::getKeyId(const std::string& str)
{
    return mKeyMap[StringUtils::toLower(str)];
}
