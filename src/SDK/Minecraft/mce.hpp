//
// Created by vastrakai on 7/5/2024.
//

#pragma once

#include <cstdint>

namespace mce {
    struct UUID {
        uint64_t mLow;
        uint64_t mHigh;
    };
    struct Color {
        float r;
        float g;
        float b;
        float a;
    };
}