#pragma once
#include <Hook/Hook.hpp>
#include <Hook/HookManager.hpp>
//
// Created by vastrakai on 6/29/2024.
//

class MouseHook : public Hook
{
    // MouseDevice *this, char actionButtonId, char buttonData, __int16 x, __int16 y, __int16 dx, __int16 dy, bool forceMotionlessPointer)
public:
    MouseHook() : Hook() {

    }

    static std::unique_ptr<Detour> mDetour;

    static void onMouse(void* _this, char actionButtonId, int buttonData, __int16 x, __int16 y, __int16 dx, __int16 dy, bool forceMotionlessPointer);
    void init() override;
};

REGISTER_HOOK(MouseHook);