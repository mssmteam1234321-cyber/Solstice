//
// Created by vastrakai on 6/24/2024.
//

#include "Solstice.hpp"

#include <iostream>
#include <thread>
#include <SDK/OffsetProvider.hpp>
#include <SDK/SigManager.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/MinecraftGame.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <Utils/Logger.hpp>
#include <spdlog/spdlog.h>
#include <Utils/ChatUtils.hpp>
#include <Utils/ProcUtils.hpp>
#include <magic_enum.hpp>
#include <MinHook.h>
#include <Features/FeatureManager.hpp>
#include <Hook/HookManager.hpp>
#include "spdlog/sinks/stdout_color_sinks-inl.h"

void Solstice::init(HMODULE hModule)
{
    // Not doing this could cause crashes if you inject too soon
    // Honestly, I don't think this helps much but it's not a bad idea to have it here
    while (ProcUtils::getModuleCount() < 130) Sleep(1);

    mModule = hModule;
    mInitialized = true;


    Logger::initialize();

    console = spdlog::stdout_color_mt(CC(21, 207, 148) + "solstice" + ANSI_COLOR_RESET, spdlog::color_mode::automatic);

    spdlog::set_pattern("[" + CC(255, 135, 0) + "%H:%M:%S.%e" + ANSI_COLOR_RESET + "] [%^%l%$%$%#%$] %v");
    console->set_pattern("[" + CC(255, 135, 0) + "%H:%M:%S.%e" + ANSI_COLOR_RESET + "] [%n] [%^%l%$%$%#%$] %v");
    console->set_level(spdlog::level::trace);
    console->info("Welcome to " + CC(0, 255, 0) + "Solstice" + ANSI_COLOR_RESET + "!"
#ifdef __DEBUG__
        + CC(255, 0, 0) + " [Debug] " + ANSI_COLOR_RESET
#endif
);

    if (MH_Initialize() != MH_OK)
    {
        console->critical("Failed to initialize MinHook!");
    }

    console->info("initializing offsetprovider...");
    OffsetProvider::initialize();

    console->info("initializing sigmanager...");
    SigManager::initialize();



    if (!ClientInstance::get())
    {
        while (!ClientInstance::get()) std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    console->info("clientinstance addr @ 0x{:X}", reinterpret_cast<uintptr_t>(ClientInstance::get()));
    console->info("mcgame from clientinstance addr @ 0x{:X}", reinterpret_cast<uintptr_t>(ClientInstance::get()->getMinecraftGame()));
    console->info("localplayer addr @ 0x{:X}", reinterpret_cast<uintptr_t>(ClientInstance::get()->getLocalPlayer()));

    console->info("initializing hooks...");
    HookManager::init();

    gFeatureManager = std::make_unique<FeatureManager>();
    gFeatureManager->init();

    ClientInstance::get()->getMinecraftGame()->playUi("beacon.activate", 1, 1.0f);
    ChatUtils::displayClientMessage("Initialized!");

    console->info("Press END to eject dll.");

    // Wait for the user to press END
    while (!mRequestEject) std::this_thread::sleep_for(std::chrono::milliseconds(1));

    mRequestEject = true;

    // Shutdown
    console->warn("Shutting down...");

    // Remove all hooks
    MH_Uninitialize();

    ClientInstance::get()->getMinecraftGame()->playUi("beacon.deactivate", 1, 1.0f);
    ChatUtils::displayClientMessage("§cEjected!");
    mInitialized = false;
    Logger::deinitialize();
    FreeLibraryAndExitThread(mModule, 0);
}

void Solstice::shutdownThread()
{
    while (mInitialized) std::this_thread::sleep_for(std::chrono::milliseconds(1));


}
