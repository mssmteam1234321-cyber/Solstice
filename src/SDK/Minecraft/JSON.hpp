//
// Created by vastrakai on 7/12/2024.
//

#pragma once

#include <map>
#include <string>
#include <string_view>


namespace MinecraftJson
{
    class Value { // equivalent to bds' Json::Value, but we dont wanna confuse it with nlohmann json
        char pad[16];
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