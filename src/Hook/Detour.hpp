#pragma once
//
// Created by vastrakai on 6/24/2024.
//


#include <string>


class Detour {
public:
    void* mFunc;
    void* mOriginalFunc{};

    std::string mName;

    ~Detour();
    Detour(const std::string& name, void* addr, void* detour);

    void Enable() const;
    void Restore() const;

    template <typename TRet, typename... TArgs>
    inline auto* GetFastcall() {
        using Fn = TRet(__fastcall*)(TArgs...);
        return reinterpret_cast<Fn>(mOriginalFunc);
    }
};
