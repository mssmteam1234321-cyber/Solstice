//
// Created by vastrakai on 7/5/2024.
//

#pragma once

#include <cstdint>
#include <Utils/MiscUtils/MathUtils.hpp>
#include <Utils/SysUtils/SHA256.hpp>

namespace mce {
    struct UUID {
        uint64_t mLow;
        uint64_t mHigh;

        static UUID generate()
        {
            UUID uuid = {};
            uuid.mLow = rand();
            uuid.mHigh = rand();
            return uuid;
        }

        [[nodiscard]] std::string toString() const
        {
            // UUID parts
            uint32_t timeLow = (mLow & 0xFFFFFFFF);
            uint16_t timeMid = (mLow >> 32) & 0xFFFF;
            uint16_t timeHiAndVersion = ((mLow >> 48) & 0x0FFF) | (4 << 12); // Set version to 4
            uint16_t clkSeq = (mHigh & 0x3FFF) | 0x8000; // Set variant to 2
            uint16_t nodeHi = (mHigh >> 16) & 0xFFFF;
            uint32_t nodeLow = (mHigh >> 32);

            std::stringstream ss;
            ss << std::hex << std::setfill('0')
               << std::setw(8) << timeLow << '-'
               << std::setw(4) << timeMid << '-'
               << std::setw(4) << timeHiAndVersion << '-'
               << std::setw(4) << clkSeq << '-'
               << std::setw(4) << nodeHi
               << std::setw(8) << nodeLow;

            return ss.str();
        }

        [[nodiscard]] bool operator==(const UUID& other) const
        {
            return mLow == other.mLow && mHigh == other.mHigh;
        }

        [[nodiscard]] bool operator!=(const UUID& other) const
        {
            return !(*this == other);
        }

        [[nodiscard]] bool operator<(const UUID& other) const
        {
            return mLow < other.mLow || (mLow == other.mLow && mHigh < other.mHigh);
        }

        [[nodiscard]] bool operator>(const UUID& other) const
        {
            return mLow > other.mLow || (mLow == other.mLow && mHigh > other.mHigh);
        }

        [[nodiscard]] bool operator<=(const UUID& other) const
        {
            return mLow < other.mLow || (mLow == other.mLow && mHigh <= other.mHigh);
        }

        [[nodiscard]] bool operator>=(const UUID& other) const
        {
            return mLow > other.mLow || (mLow == other.mLow && mHigh >= other.mHigh);
        }

        [[nodiscard]] bool operator!() const
        {
            return mLow == 0 && mHigh == 0;
        }

        [[nodiscard]] explicit operator bool() const
        {
            return mLow != 0 || mHigh != 0;
        }

        static UUID fromString(const std::string& str)
        {
            UUID uuid;
            std::string hashed = SHA256::hash(str);
            uuid.mLow = std::stoull(hashed.substr(0, 16), nullptr, 16);
            uuid.mHigh = std::stoull(hashed.substr(16, 16), nullptr, 16);
            return uuid;
        }
    };

    struct Color {
        float r;
        float g;
        float b;
        float a;

        Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {};
        Color() : r(0), g(0), b(0), a(0) {};
    };

    class ResourceLocation {
    public:
        uint64_t mType;              //0x0000
        std::string mFilePath = "";  //0x0008

        ResourceLocation(std::string filePath, bool external) {
            memset(this, 0, sizeof(ResourceLocation));
            this->mFilePath = std::move(filePath);
            if (external)
                this->mType = 2;
        };
    };

    class TexturePtr {
    public:
        PAD(0x10);
        std::shared_ptr<ResourceLocation> mTexture = nullptr;
    };
}
