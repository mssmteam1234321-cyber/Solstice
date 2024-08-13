//
// Created by vastrakai on 7/12/2024.
//

#pragma once

#include <map>
#include <string>
#include <string_view>


namespace MinecraftJson
{
    enum class ValueType : int32_t {
        Null    = 0x0,
        Int     = 0x1,
        Uint    = 0x2,
        Real    = 0x3,
        String  = 0x4,
        Boolean = 0x5,
        Array   = 0x6,
        Object  = 0x7,
    };

    class Value { // equivalent to bds' Json::Value, but we dont wanna confuse it with nlohmann json
    public:
        struct CZString {
            enum class DuplicationPolicy : unsigned int { // mPolicy?? got this from BDS
                noDuplication   = 0,
                duplicate       = 1,
                duplicateOnCopy = 2,
                Mask            = 3,
            };

            struct StringStorage {
                DuplicationPolicy mPolicy : 2;     // this+0x0
                unsigned int      mLength : 30;    // this+0x0 // 1GB Max cuz this sux
            };

            const char *mStr;       // this+0x4
            uint32_t mIndex;        // this+0x8 // ArrayIndex    mIndex{};
            StringStorage mStorage; // this+0xC

            CZString() : mStr(nullptr), mIndex(0) {}
            CZString(const char *s, uint32_t index) : mStr(s), mIndex(index) {}
        };

        union ValueHolder {
            int64_t                   mInt;       // this+0x0
            uint64_t                  mUInt64;    // this+0x8 // idk
            long double               mReal;      // this+0x10
            bool                      mBool;      // this+0x18
            char                      *mString;   // this+0x1C
            std::map<CZString, Value> *mMap;      // this+0x20
        };

        ValueHolder mValue;                       // this+0x0000

        ValueType   mType : 8;                    // this+0x0028
        int32_t     mAllocated : 1;               // this+0x002C
    public:
        std::string* toStyledString()
        {
            static uintptr_t func = SigManager::JSON_toStyledString;
            return MemUtils::callFastcall<std::string*, void*>(func, this);
        }
        nlohmann::json toNlohmannJson()
        {

        }
        static inline MinecraftJson::Value fromNlohmannJson(nlohmann::json const &json)
        {

        }
        Value() {
            memset(this, 0, sizeof(*this));
        }
    };

    class Reader {
        char pad[192];
    public:
        void Parse(std::string* input, MinecraftJson::Value* output)
        {
            static uintptr_t func = SigManager::JSON_parse;
            MemUtils::callFastcall<void, void*, std::string*, void*>(func, this, input, output);
        }
        Reader() {
            memset(this, 0, sizeof(*this));
        }
    };

};