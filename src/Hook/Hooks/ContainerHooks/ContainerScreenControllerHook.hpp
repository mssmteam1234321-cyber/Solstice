#pragma once
#include <Hook/Hook.hpp>
#include <Hook/HookManager.hpp>
//
// Created by vastrakai on 7/5/2024.
//



class ContainerScreenControllerHook : public Hook {
public:
    ContainerScreenControllerHook() : Hook() {

    }

    static std::unique_ptr<Detour> mDetour;

    static uint32_t onContainerTick(class ContainerScreenController *csc);
    void init() override;
};

