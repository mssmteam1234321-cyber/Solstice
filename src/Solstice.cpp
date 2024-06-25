//
// Created by vastrakai on 6/24/2024.
//

#include "Solstice.hpp"

#include <iostream>
#include <ostream>
#include <thread>
#include <SDK/SigManager.hpp>
#include <Utils/Logger.hpp>
#include <spdlog/spdlog.h>

#include "spdlog/sinks/stdout_color_sinks-inl.h"

void Solstice::init(HMODULE hModule)
{
    mModule = hModule;
    mInitialized = true;

    console = spdlog::stdout_color_mt("solstice", spdlog::color_mode::always);
    console->set_level(spdlog::level::trace);
    console->info("Welcome to " + CC(0, 255, 0) + "Solstice" + ANSI_COLOR_RESET + "!");
    console->info("Press END to eject dll.");
    console->trace("This is a trace message.");
    console->debug("This is a debug message.");
    console->info("This is an info message.");
    console->warn("This is a warning message.");
    console->error("This is an error message.");
    console->critical("This is a critical message.");

    console->info("initializing sigmanager...");
    SigManager::initialize();
    console->info("sigmanager initialized!");


    // Wait for the user to press END
    while (!GetAsyncKeyState(VK_END)) std::this_thread::sleep_for(std::chrono::milliseconds(1));

    // Shutdown
    console->warn("Shutting down...");

    mInitialized = false;

    Logger::deinitialize();

    FreeLibraryAndExitThread(mModule, 0);
}

void Solstice::shutdownThread()
{
    while (mInitialized) std::this_thread::sleep_for(std::chrono::milliseconds(1));


}
