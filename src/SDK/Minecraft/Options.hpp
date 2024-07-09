#pragma once
//
// Created by vastrakai on 7/8/2024.
//

#include <vector>
#include <Utils/MemUtils.hpp>


class IntOption {
public:
    char padding_16[16];
    int maximum;
    char padding_24[4];
    int value;
    char padding_32[4];
    int minimum;
};

class FloatOption
{
public:
    PAD(0x10);
    float minimum;
    float maximum;
    float value;
    float default_value;
};

class Options
{
public:
    CLASS_FIELD(IntOption*, game_thirdperson, 0x30);
    CLASS_FIELD(void*, view_bob, 0x120);
    CLASS_FIELD(FloatOption*, gfx_field_of_view, 0x1A0);
    CLASS_FIELD(FloatOption*, gfx_gamma, 0x1B8);
};
