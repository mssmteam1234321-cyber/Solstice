//
// Created by vastrakai on 7/5/2024.
//

#pragma once

#include <cstdint>

namespace mce {
    struct UUID {
        uint64_t mLow;
        uint64_t mHigh;

        [[nodiscard]] std::string toString() const
        {
            std::string result;
            result += std::to_string(mLow & 0xFFFFFFFF);
            result += "-";
            result += std::to_string((mLow >> 32) & 0xFFFF);
            result += "-";
            result += std::to_string(((mLow >> 48) & 0x0FFF) | 0x4000);
            result += "-";
            result += std::to_string(((mHigh >> 48) & 0x0FFF) | 0x8000);
            result += "-";
            result += std::to_string(mHigh & 0xFFFFFFFFFFFF);
            return result;
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