#pragma once
//
// Created by vastrakai on 7/7/2024.
//
#include <Hook/Hook.hpp>
#include <Hook/HookManager.hpp>




class SetupAndRenderHook : public Hook {
public:
    SetupAndRenderHook() : Hook() {
        mName = "ScreenView::setupAndRender";
    }

    static std::unique_ptr<Detour> mDetour;

    static void onSetupAndRender(void* screenView, void* mcuirc);
    void init() override;
};

