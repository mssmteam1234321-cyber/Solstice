//
// Created by alteik on 25/09/2024.
//

#pragma once
#include <windows.h>
#include <iostream>

class EditionFaker : public ModuleBase<EditionFaker> {
public:
    enum class OS {
        Unknown,
        Android,
        iOS,
        macOS,
        FireOs,
        GearVR,
        HoloLens,
        Windows10,
        Windows,
        Dedicated,
        Orbis,
        NX
    };

    enum class InputMethod {
        Unknown,
        Mouse,
        Touch,
        GamePad,
        MotionController
    };

    EnumSettingT<OS> mOs = EnumSettingT<OS>("OS", "Operating System to which should spoof", OS::Windows10, "Unknown", "Android", "iOS", "macOS", "FireOs", "GearVR", "HoloLens", "Windows 10", "Windows", "Dedicated", "Orbis", "NX");
    EnumSettingT<InputMethod> mInputMethod = EnumSettingT<InputMethod>("Input Method", "Input Method to which should spoof", InputMethod::Mouse,  "Unknown", "Mouse", "Touch", "GamePad", "MotionController");

    EditionFaker() : ModuleBase("EditionFaker", "spoofs ur os & input method", ModuleCategory::Misc, 0, false)
    {
        addSetting(&mOs);
        addSetting(&mInputMethod);

        mNames = {
                {Lowercase, "editionfaker"},
                {LowercaseSpaced, "edition faker"},
                {Normal, "EditionFaker"},
                {NormalSpaced, "Edition Faker"}
        };
    }

    bool isInjected = false;

    static inline unsigned char originalDefaultInputMode[31];
    static inline unsigned char originalData[sizeof(int32_t)];
    static inline unsigned char originalData1[5];
    static inline unsigned char originalData2[5];
    static inline unsigned char originalData3[5];

    static inline unsigned char patch1[] = { 0x48, 0xBF, 0x01, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x8B, 0xD7, 0x48, 0x8B, 0xCE };
    static inline unsigned char patch2[] = { 0x48, 0xBF, 0x01, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x49, 0x8B, 0x06, 0x8B, 0xD7 };
    static inline unsigned char patch3[] = { 0x48, 0xB8, 0x01, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x48, 0x8B, 0x5C, 0x24, 0x30 };

    static inline void* patch1Ptr = nullptr;
    static inline void* patch2Ptr = nullptr;
    static inline void* patch3Ptr = nullptr;

    void inject();
    void eject();
    void spoofEdition();

    void onInit() override;
    void onEnable() override;
    void onDisable() override;
    void onPacketOutEvent(class PacketOutEvent& event);
    void onConnectionRequestEvent(class ConnectionRequestEvent& event);
};