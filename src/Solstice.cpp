//
// Created by vastrakai on 6/24/2024.
//

#include "Solstice.hpp"


#include <fstream>
#include <Features/FeatureManager.hpp>
#include <Features/Configs/ConfigManager.hpp>
#include <Hook/HookManager.hpp>
#include <Hook/Hooks/RenderHooks/D3DHook.hpp>

#include <SDK/OffsetProvider.hpp>

#include <SDK/SigManager.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/MinecraftGame.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <spdlog/spdlog.h>

#include "spdlog/sinks/stdout_color_sinks-inl.h"
#include <winrt/base.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.UI.Core.h>
#include <build_info.h>
#include <Features/IRC/IrcClient.hpp>
#include <Features/Modules/Misc/IRC.hpp>
#include <Utils/OAuthUtils.hpp>

std::string title = "[" + std::string(SOLSTICE_BUILD_VERSION) + "-" + std::string(SOLSTICE_BUILD_BRANCH) + "]";

void setTitle(std::string title)
{
    auto w = winrt::Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().Dispatcher().RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, [title]() {
        winrt::Windows::UI::ViewManagement::ApplicationView::GetForCurrentView().Title(winrt::to_hstring(title));
    });
}

std::vector<unsigned char> gBpBytes = {0x1c}; // Defines the new offset for mInHandSlot
DEFINE_PATCH_FUNC(patchInHandSlot, SigManager::ItemInHandRenderer_renderItem_bytepatch2+2, gBpBytes);

void Solstice::init(HMODULE hModule)
{
    // Not doing this could cause crashes if you inject too soon
    // Honestly, I don't think this helps much but it's not a bad idea to have it here
    while (ProcUtils::getModuleCount() < 130) std::this_thread::sleep_for(std::chrono::milliseconds(1));

    int64_t start = NOW;

    mModule = hModule;
    mInitialized = true;

#ifdef __DEBUG__
    Logger::initialize();
#endif

    console = spdlog::stdout_color_mt(CC(21, 207, 148) + "solstice" + ANSI_COLOR_RESET, spdlog::color_mode::automatic);

    spdlog::set_pattern("[" + CC(255, 135, 0) + "%H:%M:%S.%e" + ANSI_COLOR_RESET + "] [%^%l%$%$%#%$] %v");
    console->set_pattern("[" + CC(255, 135, 0) + "%H:%M:%S.%e" + ANSI_COLOR_RESET + "] [%n] [%^%l%$%$%#%$] %v");
    console->set_level(spdlog::level::trace);
    spdlog::set_level(spdlog::level::trace);


    console->info("Welcome to " + CC(0, 255, 0) + "Solstice" + ANSI_COLOR_RESET + "!"
#ifdef __DEBUG__
        + CC(255, 0, 0) + " [Debug] " + ANSI_COLOR_RESET
#endif
);

    spdlog::info("Minecraft version: {}", ProcUtils::getVersion());

    ExceptionHandler::init();

    FileUtils::validateDirectories();

    // Change the window title


    setTitle(title);


    std::string lastHwidFile = FileUtils::getSolsticeDir() + xorstr_("lasthwid.txt");

    if (FileUtils::fileExists(lastHwidFile))
    {
        std::ifstream file(lastHwidFile);
        std::string lastHwid;
        file >> lastHwid;
        file.close();

        if (lastHwid != GET_HWID().toString())
        {
            FileUtils::deleteFile(lastHwidFile);
        }
    }
    else
    {
        std::ofstream file(lastHwidFile);
        file << GET_HWID().toString();
        file.close();
    }

    sHWID = GET_HWID().toString();

    if (MH_Initialize() != MH_OK)
    {
        console->critical("Failed to initialize MinHook!");
    }


    Prefs = PreferenceManager::load();

    HWND hwnd = ProcUtils::getMinecraftWindow(); // Cache the window handle

#ifdef __DEBUG__
    if (Prefs->mEnforceDebugging)
    {
        while (!IsDebuggerPresent())
        {
            MessageBoxA(nullptr, "Please attach a debugger to continue.\nThis message is being shown because of your preference settings.", "Solstice", MB_OK | MB_ICONERROR);
            spdlog::info("Waiting for debugger...");
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
#endif

    console->info("initializing signatures...");
    int64_t sstart = NOW;
    OffsetProvider::initialize();
    SigManager::initialize();
    int64_t send = NOW;

    int failedSigs = 0;

    // Go through all signatures and print any that failed
    for (const auto& [sig, result] : SigManager::mSigs)
    {
        if (result == 0)
        {
            console->critical("[signatures] Failed to find signature: {}", sig);
            failedSigs++;
        }
    }

    for (const auto& [sig, result] : OffsetProvider::mSigs)
    {
        if (result == 0)
        {
            console->critical("[offsets] Failed to find offset: {}", sig);
            failedSigs++;
        }
    }

    if (failedSigs > 0)
    {
        console->critical("Failed to find {} signatures/offsets!", failedSigs);
        Sleep(500);
    }

    console->info("initialized signatures in {}ms", send - sstart);




    if (!ClientInstance::get())
    {
        while (!ClientInstance::get()) std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    console->info("clientinstance addr @ 0x{:X}", reinterpret_cast<uintptr_t>(ClientInstance::get()));
    console->info("mcgame from clientinstance addr @ 0x{:X}", reinterpret_cast<uintptr_t>(ClientInstance::get()->getMinecraftGame()));
    console->info("localplayer addr @ 0x{:X}", reinterpret_cast<uintptr_t>(ClientInstance::get()->getLocalPlayer()));


    gFeatureManager = std::make_shared<FeatureManager>();
    gFeatureManager->init();

    console->info("initializing hooks...");
    HookManager::init(false);

    console->info("initialized in {}ms", NOW - start);

    while (!ImGui::GetCurrentContext()) std::this_thread::sleep_for(std::chrono::milliseconds(1));

    ClientInstance::get()->getMinecraftGame()->playUi("beacon.activate", 1, 1.0f);
    ChatUtils::displayClientMessage("Initialized!");

    if (!OAuthUtils::hasValidToken()) {
        ChatUtils::displayClientMessage("§eWarning: Discord not authenticated!");
        ChatUtils::displayClientMessage("§ePlease authenticate using the injector.");
        ChatUtils::displayClientMessage("§eAuthenticating with the Injector is necessary for IRC.");
    }

    // Create a thead to wait for all futures in hooks then load a default config if any
    console->info("Press END to eject dll.");

    // Create a thread to monitor the mLastTick variable
    mLastTick = NOW;

    // Wait for the user to press END
    bool firstCall = true;
    bool isLpValid = false;
    while (!mRequestEject)
    {
        if (firstCall)
        {
            NotifyUtils::notify("Solstice initialized!", 5.0f, Notification::Type::Info);
            firstCall = false;
        }

        if (!isLpValid && ClientInstance::get()->getLocalPlayer())
        {
            isLpValid = true;
            HookManager::init(true); // Initialize the base tick hook

            auto ircModule = gFeatureManager->mModuleManager->getModule<IRC>();
            if (!ircModule->mEnabled) ircModule->toggle();

            if (!Prefs->mDefaultConfigName.empty())
            {
                if (ConfigManager::configExists(Prefs->mDefaultConfigName))
                {
                    ConfigManager::loadConfig(Prefs->mDefaultConfigName);
                }
                else
                {
                    console->warn("Default config does not exist! Clearing default config...");
                    NotifyUtils::notify("Default config does not exist! Clearing default config...", 10.0f, Notification::Type::Warning);
                    Prefs->mDefaultConfigName = "";
                    PreferenceManager::save(Prefs);
                }
            } else {
                console->warn("No default config set!");
            }
        }

        patchInHandSlot(ClientInstance::get()->getLocalPlayer() != nullptr);

        mLastTick = NOW;
        gFeatureManager->mModuleManager->onClientTick();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }



    mRequestEject = true;

    setTitle("");

    patchInHandSlot(false);

    HookManager::shutdown();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    gFeatureManager->shutdown();

    // Shutdown
    console->warn("Shutting down...");

    ClientInstance::get()->getMinecraftGame()->playUi("beacon.deactivate", 1, 1.0f);
    ChatUtils::displayClientMessage("§cEjected!");

    mInitialized = false;
    SigManager::deinitialize();
    OffsetProvider::deinitialize();

    // wait for threads to finish
    Sleep(1000); // Give the user time to read the message

    Logger::deinitialize();
    FreeLibraryAndExitThread(mModule, 0);
}

void Solstice::shutdownThread()
{
    while (mInitialized) std::this_thread::sleep_for(std::chrono::milliseconds(1));


}
